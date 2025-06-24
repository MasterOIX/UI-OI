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

    QDBusConnection::systemBus().connect(
        "org.bluez",
        QString(),
        "org.freedesktop.DBus.ObjectManager",
        "InterfacesAdded",
        this,
        SLOT(onInterfacesAdded(QDBusObjectPath,QVariantMap))
    );
    QDBusConnection::systemBus().connect(
        "org.bluez",
        QString(),
        "org.freedesktop.DBus.ObjectManager",
        "InterfacesRemoved",
        this,
        SLOT(onInterfacesRemoved(QString,QStringList))
    );
}

void BluetoothAudioSource::setBluezDevicePath(const QString &path) {
    stop();

    if (path.isEmpty()) {
        qWarning() << "[BluetoothAudioSource] Device path is empty! Resetting paths.";
        m_bluezDevicePath.clear();
        m_playerPath.clear();
        m_currentTrack.clear();
        return;
    }

    m_bluezDevicePath = path;
    m_playerPath.clear();
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
    if (m_volumeElement) {
        gst_object_unref(m_volumeElement);
        m_volumeElement = nullptr;
    }

    m_playerPath.clear();
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
            callControlMethod("Play");
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
      "alsasrc device=%1 buffer-time=200000 latency-time=20000 provide-clock=false ! "
      "queue ! "
      "audioconvert ! audioresample ! "
      "volume name=btvolume volume=1.0 ! "
      "alsasink device=plughw:sndrpihifiberry").arg(deviceLine);


    qDebug() << "[BluetoothAudioSource] Starting GStreamer pipeline:" << pipelineStr;

    m_gstPipeline = gst_parse_launch(pipelineStr.toUtf8().constData(), nullptr);
    m_volumeElement = gst_bin_get_by_name(GST_BIN(m_gstPipeline), "btvolume");
    if (m_gstPipeline) {
        gst_element_set_state(m_gstPipeline, GST_STATE_PLAYING);
        qDebug() << "[BluetoothAudioSource] GStreamer pipeline created and started.";
        GstBus *bus = gst_element_get_bus(m_gstPipeline);
        gst_bus_add_watch(bus, [](GstBus *bus, GstMessage *msg, gpointer user_data) -> gboolean {
            Q_UNUSED(bus);
            auto *self = static_cast<BluetoothAudioSource*>(user_data);

            switch (GST_MESSAGE_TYPE(msg)) {
            case GST_MESSAGE_EOS:
                qDebug() << "[BluetoothAudioSource] GStreamer stream ended. Cleaning up.";
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
        QTimer::singleShot(1000, this, &BluetoothAudioSource::play); // evită apeluri rapide
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

    QDBusInterface properties("org.bluez", m_bluezDevicePath, "org.freedesktop.DBus.Properties", QDBusConnection::systemBus());
    QDBusReply<QVariant> connectedReply = properties.call("Get", "org.bluez.MediaControl1", "Connected");

    if (connectedReply.isValid() && connectedReply.value().toBool()) {
        QDBusInterface control("org.bluez", m_bluezDevicePath, "org.bluez.MediaControl1", QDBusConnection::systemBus());
        if (control.isValid()) {
            QDBusMessage reply = control.call(method);
            if (reply.type() == QDBusMessage::ErrorMessage)
                qWarning() << "[BluetoothAudioSource] MediaControl1:" << method << "failed:" << reply.errorMessage();
            else
                qDebug() << "[BluetoothAudioSource] MediaControl1:" << method << "command sent.";
            return;
        }
    }

    // Fallback la MediaPlayer1
    if (m_playerPath.isEmpty()) {
        qWarning() << "[BluetoothAudioSource] Cannot call" << method << "– player path is empty!";
        return;
    }

    QDBusInterface control("org.bluez", m_playerPath, "org.bluez.MediaPlayer1", QDBusConnection::systemBus());
    if (!control.isValid()) {
        qWarning() << "[BluetoothAudioSource] MediaPlayer1 interface invalid at" << m_playerPath;
        return;
    }

    QDBusMessage reply = control.call(method);
    if (reply.type() == QDBusMessage::ErrorMessage)
        qWarning() << "[BluetoothAudioSource] MediaPlayer1:" << method << "failed:" << reply.errorMessage();
    else
        qDebug() << "[BluetoothAudioSource] MediaPlayer1:" << method << "command sent.";
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
    if (!m_volumeElement) {
        qWarning() << "[BluetoothAudioSource] Volume element not available!";
        return;
    }

    double volume = qBound(0.0, volumePercent / 100.0, 1.0);
    g_object_set(G_OBJECT(m_volumeElement), "volume", volume, nullptr);
    qDebug() << "[BluetoothAudioSource] Set GStreamer volume to:" << volume;
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

    if (interface == "org.bluez.MediaControl1" && changedProps.contains("Connected")) {
        bool connected = changedProps.value("Connected").toBool();

        if (connected) {
            qDebug() << "[BluetoothAudioSource] MediaControl1 connected";
            if (!m_playerPath.isEmpty()) {
                play();
            }
        }
    }

    if (interface == "org.bluez.Device1" && changedProps.contains("Connected")) {
        bool connected = changedProps.value("Connected").toBool();
        qDebug() << "[BluetoothAudioSource] Device connection state changed:" << connected;

        if (!connected) {
            qDebug() << "[BluetoothAudioSource] Device disconnected. Cleaning up.";
            stop();
            return;
        }
    }
}


void BluetoothAudioSource::getTrack() {
    if (m_playerPath.isEmpty()) {
        qWarning() << "[BluetoothAudioSource] Cannot get track – player path is empty!";
        return;
    }

    QDBusInterface props("org.bluez", m_playerPath,
                         "org.freedesktop.DBus.Properties",
                         QDBusConnection::systemBus());

    QDBusReply<QVariant> trackReply = props.call("Get", "org.bluez.MediaPlayer1", "Track");
    if (trackReply.isValid()) {
        m_currentTrack = qdbus_cast<QVariantMap>(trackReply.value());
        emit metadataChanged();
        qDebug() << "[BluetoothAudioSource] Track info updated:" << m_currentTrack;
    } else {
        qWarning() << "[BluetoothAudioSource] Failed to get Track metadata:" << trackReply.error().message();
    }
}

void BluetoothAudioSource::onInterfacesAdded(const QDBusObjectPath &objectPath, const QVariantMap &interfaces) {
    if (interfaces.contains("org.bluez.Device1")) {
        m_bluezDevicePath = objectPath.path();
        qDebug() << "[BluetoothAudioSource] Device added at path:" << m_bluezDevicePath;
        emit metadataChanged();
    }
}

void BluetoothAudioSource::onInterfacesRemoved(const QString &objectPath, const QStringList &interfaces) {
    if (interfaces.contains("org.bluez.Device1") && objectPath == m_bluezDevicePath) {
        qDebug() << "[BluetoothAudioSource] Device removed at path:" << objectPath;
        m_bluezDevicePath.clear();
        m_playerPath.clear();
        m_currentTrack.clear();
        emit metadataChanged();
    }
}


