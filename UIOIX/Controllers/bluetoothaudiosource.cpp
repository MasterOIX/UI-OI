#include "bluetoothaudiosource.h"
#include <QDebug>
#include <QDBusInterface>
#include <QDBusConnection>
#include <QDBusReply>
#include <QTimer>

BluetoothAudioSource::BluetoothAudioSource(QObject *parent) : AudioSource(parent) {
    getTrack();
    emit metadataChanged();
    emit playbackInfoChanged();
    QDBusConnection::systemBus().connect(
        "org.bluez",
        QString(),
        "org.freedesktop.DBus.Properties",
        "PropertiesChanged",
        this,
        SLOT(onPropertiesChanged(QString,QVariantMap,QStringList))
    );
}

void BluetoothAudioSource::setBluezDevicePath(const QString &path) {
    qDebug() << "[BluetoothAudioSource] Setting BlueZ device path to:" << path;

    if (path.isEmpty()) {
        qWarning() << "[BluetoothAudioSource] Device path is empty! Resetting paths.";
        m_bluezDevicePath.clear();
        m_playerPath.clear();
        m_currentTrack.clear();
        return;
    }

    m_bluezDevicePath = path;
    m_playerPath.clear();

    // Connect to ServicesResolved property changes
    QDBusConnection::systemBus().connect(
        "org.bluez",
        m_bluezDevicePath,
        "org.freedesktop.DBus.Properties",
        "PropertiesChanged",
        this,
        SLOT(onDevicePropertiesChanged(QString,QVariantMap,QStringList))
        );

    // Interface to read DBus properties
    QDBusInterface iface(
        "org.bluez",
        m_bluezDevicePath,
        "org.freedesktop.DBus.Properties",
        QDBusConnection::systemBus()
        );

    // Check if services are ready
    QDBusReply<QVariant> resolvedReply = iface.call("Get", "org.bluez.Device1", "ServicesResolved");
    if (!resolvedReply.isValid()) {
        qWarning() << "[Bluetooth] Could not query ServicesResolved:" << resolvedReply.error().message();
        return;
    }

    if (!resolvedReply.value().toBool()) {
        qWarning() << "[Bluetooth] Device not ready: ServicesResolved is false";
        return;
    }

    // Now get the player path via MediaControl1
    QDBusReply<QVariant> reply = iface.call("Get", "org.bluez.MediaControl1", "Player");
    if (reply.isValid()) {
        QDBusObjectPath playerPath = reply.value().value<QDBusObjectPath>();
        m_playerPath = playerPath.path();
        qDebug() << "[BluetoothAudioSource] Found player path:" << m_playerPath;
    } else {
        qWarning() << "[Bluetooth] Failed to get Player:" << reply.error().message();
    }

    if (m_playerPath.isEmpty()) {
        qWarning() << "[BluetoothAudioSource] No MediaPlayer1 interface found under device path:" << m_bluezDevicePath;
    } else {
        getTrack();  // Fetch initial track metadata
        qDebug() << "[BluetoothAudioSource] BlueZ device path set to:" << m_bluezDevicePath;
        qDebug() << "[BluetoothAudioSource] BlueZ player path set to:" << m_playerPath;
    }
}


void BluetoothAudioSource::onDevicePropertiesChanged(const QString &interface,
                                                     const QVariantMap &changedProps,
                                                     const QStringList &invalidatedProps)
{
    Q_UNUSED(invalidatedProps);

    if (interface == "org.bluez.Device1" && changedProps.contains("ServicesResolved")) {
        bool resolved = changedProps.value("ServicesResolved").toBool();
        qDebug() << "[BluetoothAudioSource] ServicesResolved changed:" << resolved;

        if (resolved) {
            // Retry setting device path (updates m_playerPath too)
            setBluezDevicePath(m_bluezDevicePath);

            // ✅ Update metadata
            QDBusInterface props("org.bluez", m_playerPath, "org.freedesktop.DBus.Properties", QDBusConnection::systemBus());
            getTrack();
            QDBusReply<QVariant> posReply = props.call("Get", "org.bluez.MediaPlayer1", "Position");
            if (posReply.isValid()) {
                emit playbackInfoChanged();
            } else {
                qWarning() << "[BluetoothAudioSource] Failed to get Position:" << posReply.error().message();
            }

            // ✅ Auto-play if needed
            play();
        }
    }
}


void BluetoothAudioSource::stop() {
    if (m_bluezDevicePath.isEmpty()) {
        qWarning() << "[BluetoothAudioSource] Cannot stop – device path is empty.";
        return;
    }

    QDBusInterface deviceInterface(
        "org.bluez",
        m_bluezDevicePath,
        "org.bluez.Device1",
        QDBusConnection::systemBus()
        );

    if (!deviceInterface.isValid()) {
        qWarning() << "[BluetoothAudioSource] Invalid D-Bus interface at:" << m_bluezDevicePath;
        return;
    }

    QDBusPendingCall call = deviceInterface.asyncCall("Disconnect");
    QDBusPendingCallWatcher* watcher = new QDBusPendingCallWatcher(call, this);

    connect(watcher, &QDBusPendingCallWatcher::finished, this, [=](QDBusPendingCallWatcher* w) {
        QDBusPendingReply<> reply = *w;
        if (reply.isError()) {
            qWarning() << "[BluetoothAudioSource] Failed to disconnect device:" << reply.error().message();
        } else {
            qDebug() << "[BluetoothAudioSource] Device disconnected successfully.";
        }
        w->deleteLater();
    });
}

void BluetoothAudioSource::next() {
    callControlMethod("Next");
}

void BluetoothAudioSource::previous() {
    callControlMethod("Previous");
}

void BluetoothAudioSource::play() {
    if (m_bluezDevicePath.isEmpty()) {
        qWarning() << "[BluetoothAudioSource] Cannot play – device path is empty.";
        return;
    }

    // Check if connected
    QDBusInterface propertiesIface(
        "org.bluez",
        m_bluezDevicePath,
        "org.freedesktop.DBus.Properties",
        QDBusConnection::systemBus()
        );

    if (propertiesIface.isValid()) {
        QDBusReply<QVariant> connectedReply = propertiesIface.call("Get", "org.bluez.Device1", "Connected");

        if (connectedReply.isValid() && !connectedReply.value().toBool()) {
            // Not connected — try to connect
            QDBusInterface deviceIface(
                "org.bluez",
                m_bluezDevicePath,
                "org.bluez.Device1",
                QDBusConnection::systemBus()
                );

            if (deviceIface.isValid()) {
                deviceIface.asyncCall("Connect");
                qDebug() << "[BluetoothAudioSource] Attempted auto-connect before play.";
            }
        }
    }

    // Always try to play
    callControlMethod("Play");
}


void BluetoothAudioSource::pause() {
    callControlMethod("Pause");
}

void BluetoothAudioSource::callControlMethod(const QString &method) {
    if (m_bluezDevicePath.isEmpty()) {
        qWarning() << "[BluetoothAudioSource] Cannot call" << method << "– device path is empty!";
        return;
    }

    QDBusInterface control("org.bluez", m_playerPath, "org.bluez.MediaPlayer1", QDBusConnection::systemBus());
    if (!control.isValid()) {
        qWarning() << "[BluetoothAudioSource] MediaControl1 interface invalid at" << m_bluezDevicePath;
        return;
    }

    QDBusMessage reply = control.call(method);
    if (reply.type() == QDBusMessage::ErrorMessage)
        qWarning() << "[BluetoothAudioSource]" << method << "failed:" << reply.errorMessage();
    else
        qDebug() << "[BluetoothAudioSource]" << method << "command sent via MediaControl1";
}

QString BluetoothAudioSource::title() const {
    return m_currentTrack.value("Title").toString();
}

QString BluetoothAudioSource::artist() const {
    return m_currentTrack.value("Artist").toString();
}

QString BluetoothAudioSource::album() const {
    return m_currentTrack.value("Album").toString();
}

bool BluetoothAudioSource::queryDuration(qint64 &durationMs) const {
    QVariant durVar = m_currentTrack.value("Duration");
    if (!durVar.isValid()) return false;
    durationMs = durVar.toLongLong() / 1000;
    return true;
}

bool BluetoothAudioSource::queryPosition(qint64 &positionMs) const {
    if (m_playerPath.isEmpty()) return false;

    QDBusInterface player("org.bluez", m_playerPath, "org.freedesktop.DBus.Properties", QDBusConnection::systemBus());
    QDBusReply<QVariant> reply = player.call("Get", "org.bluez.MediaPlayer1", "Position");
    if (reply.isValid()) {
        positionMs = reply.value().toLongLong() / 1000;
        return true;
    }
    qWarning() << "[BluetoothAudioSource] Could not retrieve position!";
    return false;
}

void BluetoothAudioSource::playAt(int index) {
    Q_UNUSED(index);
}

void BluetoothAudioSource::setVolume(int volumePercent) {
    if (m_playerPath.isEmpty()) return;
    int volume = qBound(0, volumePercent * 127 / 100, 127);
    QDBusInterface player("org.bluez", m_playerPath, "org.freedesktop.DBus.Properties", QDBusConnection::systemBus());
    player.call("Set", "org.bluez.MediaPlayer1", "Volume", QVariant::fromValue(volume));
}

void BluetoothAudioSource::onPropertiesChanged(const QString &interface,
                                               const QVariantMap &changedProps,
                                               const QStringList &invalidatedProps) {
    Q_UNUSED(invalidatedProps);
    if (interface == "org.bluez.MediaPlayer1") {
        if (changedProps.contains("Track")) {
            m_currentTrack = qdbus_cast<QVariantMap>(changedProps.value("Track"));
            emit metadataChanged();
        }
    }
}

void BluetoothAudioSource::getTrack() {
    if (m_playerPath.isEmpty()) {
        qWarning() << "[BluetoothAudioSource] Cannot get track – player path is empty!";
        return;
    }

    QDBusInterface props("org.bluez", m_playerPath, "org.freedesktop.DBus.Properties", QDBusConnection::systemBus());

    QDBusReply<QVariant> trackReply = props.call("Get", "org.bluez.MediaPlayer1", "Track");
    if (trackReply.isValid()) {
        m_currentTrack = qdbus_cast<QVariantMap>(trackReply.value());
        emit metadataChanged();
        qDebug() << "[BluetoothAudioSource] Track info updated: " << m_currentTrack;
    } else {
        qWarning() << "[BluetoothAudioSource] Failed to get Track metadata:" << trackReply.error().message();
    }
}

