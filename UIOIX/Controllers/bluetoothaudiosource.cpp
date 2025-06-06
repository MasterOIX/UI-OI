#include "bluetoothaudiosource.h"
#include <QDebug>
#include <QDBusInterface>
#include <QDBusConnection>
#include <QDBusReply>
#include <QTimer>
#include <QProcess>
#include <QStringList>

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

    if (m_gstPipeline) {
        qDebug() << "[BluetoothAudioSource] Stopping existing GStreamer pipeline before setting new device path.";
        gst_element_set_state(m_gstPipeline, GST_STATE_NULL);
        gst_object_unref(m_gstPipeline);
        m_gstPipeline = nullptr;
    }

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
        if (isPlaying()) {
            play();  // Auto-play if already playing
        }
    }
}


void BluetoothAudioSource::onDevicePropertiesChanged(const QString &interface,
                                                     const QVariantMap &changedProps,
                                                     const QStringList &invalidatedProps)
{
    Q_UNUSED(invalidatedProps);

    if (interface == "org.bluez.Device1") {
        if (changedProps.contains("Connected")) {
            bool connected = changedProps.value("Connected").toBool();
            qDebug() << "[BluetoothAudioSource] Device connection state changed:" << connected;

            if (!connected) {
                qDebug() << "[BluetoothAudioSource] Device disconnected. Cleaning up.";
                stop();  // cleans up safely
                m_playerPath.clear();  // avoid using old path
                return;
            }
        }

        if (changedProps.contains("ServicesResolved")) {
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
                if (isPlaying()) {
                    play();
                }
            }
        }
    }
}

void BluetoothAudioSource::stop() {
    qDebug() << "[BluetoothAudioSource] Stopping Bluetooth audio source...";
    callControlMethod("Pause");

    // Stop GStreamer pipeline if running
    if (m_gstPipeline) {
        gst_element_set_state(m_gstPipeline, GST_STATE_NULL);
        gst_object_unref(m_gstPipeline);
        m_gstPipeline = nullptr;
        qDebug() << "[BluetoothAudioSource] GStreamer pipeline stopped and cleaned up.";
    }
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

    // Auto-connect if not already
    QDBusInterface propertiesIface("org.bluez", m_bluezDevicePath, "org.freedesktop.DBus.Properties", QDBusConnection::systemBus());
    if (propertiesIface.isValid()) {
        QDBusReply<QVariant> connectedReply = propertiesIface.call("Get", "org.bluez.Device1", "Connected");
        if (connectedReply.isValid() && !connectedReply.value().toBool()) {
            QDBusInterface deviceIface("org.bluez", m_bluezDevicePath, "org.bluez.Device1", QDBusConnection::systemBus());
            if (deviceIface.isValid()) {
                deviceIface.asyncCall("Connect");
                qDebug() << "[BluetoothAudioSource] Attempted auto-connect before play.";
            }
        }
    }

    // Resume if paused or playing
    if (m_gstPipeline) {
        GstState currentState;
        gst_element_get_state(m_gstPipeline, &currentState, nullptr, 0);
        if (currentState == GST_STATE_PAUSED || currentState == GST_STATE_PLAYING) {
            gst_element_set_state(m_gstPipeline, GST_STATE_PLAYING);
            qDebug() << "[BluetoothAudioSource] Resuming existing GStreamer pipeline.";
            return;
        } else {
            gst_element_set_state(m_gstPipeline, GST_STATE_NULL);
            gst_object_unref(m_gstPipeline);
            m_gstPipeline = nullptr;
        }
    }

    if (!gst_is_initialized()) {
        gst_init(nullptr, nullptr);
    }

    // Get first line from bluealsa-aplay -L
    QProcess proc;
    proc.start("bluealsa-aplay", QStringList() << "-L");
    proc.waitForFinished();
    QString output = proc.readAllStandardOutput();
    QStringList lines = output.split('\n', Qt::SkipEmptyParts);

    if (lines.isEmpty()) {
        qWarning() << "[BluetoothAudioSource] No output from bluealsa-aplay -L.";
        return;
    }

    QString deviceLine = lines.first().trimmed();
    qDebug() << "[BluetoothAudioSource] Using BlueALSA device:" << deviceLine;

    QString pipelineStr = QString(
      "alsasrc device=%1 provide-clock=false blocksize=2048 latency-time=100000 ! "
      "audioconvert ! "
      "audioresample ! "
      "alsasink device=hw:0").arg(deviceLine);

    qDebug() << "[BluetoothAudioSource] Starting GStreamer pipeline:" << pipelineStr;

    m_gstPipeline = gst_parse_launch(pipelineStr.toUtf8().constData(), nullptr);
    if (m_gstPipeline) {
        gst_element_set_state(m_gstPipeline, GST_STATE_PLAYING);
        qDebug() << "[BluetoothAudioSource] GStreamer pipeline created and started.";
        GstBus *bus = gst_element_get_bus(m_gstPipeline);
        gst_bus_add_watch(bus, [](GstBus *bus, GstMessage *msg, gpointer user_data) -> gboolean {
            Q_UNUSED(bus);
            auto *self = static_cast<BluetoothAudioSource*>(user_data);

            switch (GST_MESSAGE_TYPE(msg)) {
            case GST_MESSAGE_EOS:
                qDebug() << "[BluetoothAudioSource] GStreamer stream ended or errored. Cleaning up.";
                QMetaObject::invokeMethod(self, "handleStreamRestart", Qt::QueuedConnection);
                break;
            case GST_MESSAGE_ERROR:
                qDebug() << "[BluetoothAudioSource] GStreamer stream ended or errored. Cleaning up.";
                QMetaObject::invokeMethod(self, "handleStreamRestart", Qt::QueuedConnection);
                break;
            default:
                break;
            }
            return TRUE;
        }, this);
        gst_object_unref(bus);
    } else {
        qWarning() << "[BluetoothAudioSource] Failed to create GStreamer pipeline.";
        return;
    }

    callControlMethod("Play");
}

void BluetoothAudioSource::handleStreamRestart() {
    stop();
    if (isPlaying()) {
        qDebug() << "[BluetoothAudioSource] Restarting Bluetooth playback...";
        play();
    }
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
                                               const QStringList &invalidatedProps)
{
    Q_UNUSED(invalidatedProps);

    if (interface == "org.bluez.MediaPlayer1") {
        if (changedProps.contains("Track")) {
            m_currentTrack = qdbus_cast<QVariantMap>(changedProps.value("Track"));
            emit metadataChanged();
        }
    }

    // ✅ Detect change in MediaControl1's Player property
    if (interface == "org.bluez.MediaControl1" && changedProps.contains("Player")) {
        QDBusObjectPath newPlayerPath = changedProps.value("Player").value<QDBusObjectPath>();
        QString newPath = newPlayerPath.path();

        if (newPath != m_playerPath) {
            qDebug() << "[BluetoothAudioSource] Player path changed to:" << newPath;
            m_playerPath = newPath;

            // Refresh metadata if needed
            getTrack();
            emit playbackInfoChanged();
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

