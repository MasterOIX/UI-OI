#include "bluetoothcontroller.h"
#include "Controllers/bluezagent.h"
#include <QDBusConnection>
#include <QDBusReply>
#include <QDebug>
#include <QDBusObjectPath>
#include <QDBusInterface>
#include <QDBusMetaType>
#include <QDBusPendingReply>
#include <QDBusPendingCallWatcher>
#include <QVariantMap>
#include <QTimer>


BluetoothController::BluetoothController(QObject *parent)
    : QObject(parent)
{
    bluetoothEnabledAsync([this](bool powered) {
        m_bluetoothEnabled = powered;
        emit bluetoothEnabledChanged();
    });
}


void BluetoothController::bluetoothEnabledAsync(std::function<void(bool)> callback) {
    QString path = getAdapterPath();
    if (path.isEmpty()) {
        qWarning() << "[Bluetooth] No adapter found, skipping initialization.";
        callback(false);
        return;
    }

    QDBusInterface adapter("org.bluez", path, "org.freedesktop.DBus.Properties", QDBusConnection::systemBus());
    if (!adapter.isValid()) {
        qWarning() << "[Bluetooth] Adapter not found at path:" << path;
        callback(false);
        return;
    }

    QDBusPendingCall pendingCall = adapter.asyncCall("Get", "org.bluez.Adapter1", "Powered");
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(pendingCall);
    watcher->setParent(this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [watcher, callback](QDBusPendingCallWatcher *) {
        QDBusPendingReply<QDBusVariant> reply = *watcher;
        bool powered = false;
        if (reply.isValid())
            powered = reply.value().variant().toBool();
        callback(powered);
        watcher->deleteLater();
    });
}


void BluetoothController::setBluetoothEnabled(bool enabled) {
    QString path = getAdapterPath();
    qDebug() << "[BluetoothController] Enabling Bluetooth adapter... path:" << path;
    if (path.isEmpty()) {
        qWarning() << "[Bluetooth] No adapter found for enabling/disabling.";
        emit bluetoothOperationPendingChanged(false);
        return;
    }

    QDBusInterface adapter("org.bluez", path, "org.freedesktop.DBus.Properties", QDBusConnection::systemBus());
    if (!adapter.isValid()) {
        qWarning() << "[Bluetooth] Invalid adapter interface at path:" << path;
        emit bluetoothOperationPendingChanged(false);
        return;
    }

    emit bluetoothOperationPendingChanged(true);

    if (enabled) {
        QVariant arg = QVariant::fromValue(QDBusVariant(QVariant(true)));
        QDBusPendingCall call = adapter.asyncCall("Set", "org.bluez.Adapter1", "Powered", arg);
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);

        connect(watcher, &QDBusPendingCallWatcher::finished, this, [=](QDBusPendingCallWatcher *w) {
            QDBusPendingReply<> reply = *watcher;
            if (reply.isError()) {
                qWarning() << "[Bluetooth] Failed to enable Bluetooth:" << reply.error().message();
                bluetoothEnabledAsync([this](bool actualPowered) {
                    m_bluetoothEnabled = actualPowered;
                    emit bluetoothEnabledChanged();
                    emit bluetoothOperationPendingChanged(false);
                });
            } else {
                m_bluetoothEnabled = true;
                emit bluetoothEnabledChanged();
                QTimer::singleShot(100, this, [this]() {
                    qDebug() << "[BluetoothController] Delayed scanPairedDevices after Bluetooth enable.";
                    scanPairedDevices();
                });
                qDebug() << "[Bluetooth] Bluetooth adapter enabled successfully.";
                emit bluetoothOperationPendingChanged(false);
            }
            w->deleteLater();
        });

    } else {
        qDebug() << "[Bluetooth] Disabling = disconnecting current device (no power off)";
        disconnectFromDevice();

        if (m_connectedDevice) {
            m_connectedDevice->setConnected(false);
            delete m_connectedDevice;
            m_connectedDevice = nullptr;
            emit connectedDeviceChanged();
        }

        clearDevices(); // <-- Add this to avoid stale pointers
        m_bluetoothEnabled = false;
        emit bluetoothEnabledChanged();
        emit bluetoothOperationPendingChanged(false);
    }

}


void BluetoothController::removePairedDevice(const QString &mac) {
    QString path = macToDevicePath(mac);
    if (path.isEmpty()) return;
    QDBusInterface adapter("org.bluez", getAdapterPath(), "org.bluez.Adapter1", QDBusConnection::systemBus());
    QDBusPendingCall asyncCall = adapter.asyncCall("RemoveDevice", QDBusObjectPath(path));
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(asyncCall, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [this, watcher](QDBusPendingCallWatcher *) {
        scanPairedDevices(); // Triggers async device list update
        watcher->deleteLater();
    });
}

void BluetoothController::scanPairedDevices() {
    qDebug() << "[BluetoothController] scanPairedDevices() triggered.";

    clearDevices();
    qDebug() << "[BluetoothController] Clearing existing paired devices.";
    QDBusInterface objManager("org.bluez", "/", "org.freedesktop.DBus.ObjectManager", QDBusConnection::systemBus());

    if (!objManager.isValid()) {
        qWarning() << "[BluetoothController] Failed to create DBus ObjectManager interface!";
        return;
    }

    QDBusPendingCall asyncCall = objManager.asyncCall("GetManagedObjects");
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(asyncCall, this);

    connect(watcher, &QDBusPendingCallWatcher::finished, this, [=](QDBusPendingCallWatcher *w) {
        Q_UNUSED(w);
        qDebug() << "[BluetoothController] GetManagedObjects finished.";
        QDBusPendingReply<QMap<QDBusObjectPath, QMap<QString, QVariantMap>>> reply = *watcher;

        if (!reply.isValid()) {
            qWarning() << "[BluetoothController] GetManagedObjects failed:" << reply.error().message();
            watcher->deleteLater();
            return;
        }

        auto managed = reply.value();
        for (auto it = managed.constBegin(); it != managed.constEnd(); ++it) {
            const QMap<QString, QVariantMap> &interfaces = it.value();

            if (interfaces.contains("org.bluez.Device1")) {
                const QVariantMap &ifaceProps = interfaces["org.bluez.Device1"];
                QString name = ifaceProps.value("Name").toString();
                QString mac = ifaceProps.value("Address").toString();
                bool connected = ifaceProps.value("Connected").toBool();
                qDebug() << "[BluetoothController] Scanning paired device:" << name << mac << "Connected:" << connected;
                auto *device = new BluetoothDevice(name, mac, connected, this);

                m_pairedDevices.append(device);
                if (connected) {
                    m_connectedDevice = device;
                    emit connectedDeviceChanged();
                    emit bluetoothMediaPlayerPathChanged(bluezPlayerPathForDevice(device->mac()));
                }
            }
        }
        //autoConnectIfAvailable();
        emit pairedDevicesChanged();
        watcher->deleteLater();
    });
}


void BluetoothController::makeDiscoverable(int seconds) {
    QString path = getAdapterPath();
    if (path.isEmpty()) {
        qWarning() << "[Bluetooth] No adapter found, skipping initialization.";
        return;
    }

    QDBusInterface adapter("org.bluez", path, "org.freedesktop.DBus.Properties", QDBusConnection::systemBus());
    if (!adapter.isValid()) {
        qWarning() << "[Bluetooth] Cannot make discoverable. Adapter not found.";
        return;
    }

    QVariant arg = QVariant::fromValue(QDBusVariant(QVariant(true)));
    adapter.asyncCall("Set", "org.bluez.Adapter1", "Discoverable", arg);

    QVariant arg2 = QVariant::fromValue(QDBusVariant(QVariant(true)));
    adapter.asyncCall("Set", "org.bluez.Adapter1", "Pairable", arg2);

    m_pairable = true;
    emit pairableChanged();

    // Disable after timer expires (async)
    m_discoverableTimer.singleShot(seconds * 1000, this, [this]() {
        QDBusInterface a("org.bluez", getAdapterPath(), "org.freedesktop.DBus.Properties", QDBusConnection::systemBus());
        QVariant off = QVariant::fromValue(QDBusVariant(QVariant::fromValue(false)));
        a.asyncCall("Set", "org.bluez.Adapter1", "Discoverable", off);
        a.asyncCall("Set", "org.bluez.Adapter1", "Pairable", off);
        m_pairable = false;
        emit pairableChanged();
    });
}

void BluetoothController::connectToPairedDevice(const QString &mac) {
    QString path = macToDevicePath(mac);
    if (path.isEmpty()) return;
    connectToDevicePath(path);
}

void BluetoothController::connectToDevicePath(const QString &devicePath) {
    QDBusInterface dev("org.bluez", devicePath, "org.bluez.Device1", QDBusConnection::systemBus());
    QDBusPendingCall asyncCall = dev.asyncCall("Connect");
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(asyncCall, this);

    connect(watcher, &QDBusPendingCallWatcher::finished, this, [=](QDBusPendingCallWatcher *) {
        QDBusPendingReply<void> reply = *watcher;
        if (reply.isError()) {
            qWarning() << "[Bluetooth] Failed to connect to device at path:" << devicePath
                       << ". Error:" << reply.error().message();
        } else {
            QDBusInterface iface("org.bluez", devicePath,
                                 "org.freedesktop.DBus.Properties",
                                 QDBusConnection::systemBus());

            QDBusReply<QVariant> reply = iface.call("Get", "org.bluez.MediaControl1", "Connected");

            if (!reply.isValid()) {
                qWarning() << "[Bluetooth] Failed to get MediaControl1 Connected property:" << reply.error().message();
                return;
            }
            if (!reply.value().toBool()) {
                QTimer::singleShot(3500, this, [=]() {
                    QDBusInterface retryDev("org.bluez", devicePath, "org.bluez.Device1", QDBusConnection::systemBus());
                    retryDev.call("Connect");

                    trustDevice(devicePath);
                    for (QObject *obj : std::as_const(m_pairedDevices)) {
                        auto *dev = qobject_cast<BluetoothDevice *>(obj);
                        if (dev && macToDevicePath(dev->mac()) == devicePath) {
                            m_connectedDevice = dev;
                            dev->setConnected(true);
                            emit connectedDeviceChanged();
                            emit bluetoothMediaPlayerPathChanged(bluezPlayerPathForDevice(dev->mac()));
                            break;
                        }
                    }
                });
            }

            else {
                qDebug() << "[Bluetooth] Successfully connected to device at path:" << devicePath;
                trustDevice(devicePath);

                // Find the device object and update internal state
                for (QObject *obj : std::as_const(m_pairedDevices)) {
                    auto *dev = qobject_cast<BluetoothDevice *>(obj);
                    if (dev && macToDevicePath(dev->mac()) == devicePath) {
                        m_connectedDevice = dev;
                        dev->setConnected(true);
                        emit connectedDeviceChanged();
                        emit bluetoothMediaPlayerPathChanged(bluezPlayerPathForDevice(dev->mac()));
                        break;
                    }
                }
            }
        }
        watcher->deleteLater();
    });
}

void BluetoothController::disconnectFromDevice() {
    if (!m_connectedDevice) return;
    QString path = macToDevicePath(m_connectedDevice->mac());
    if (path.isEmpty()) return;

    QDBusInterface dev("org.bluez", path, "org.bluez.Device1", QDBusConnection::systemBus());
    QDBusPendingCall asyncCall = dev.asyncCall("Disconnect");
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(asyncCall, this);

    connect(watcher, &QDBusPendingCallWatcher::finished, this, [=](QDBusPendingCallWatcher *) {
        QDBusPendingReply<void> reply = *watcher;
        if (reply.isError()) {
            qWarning() << "[Bluetooth] Disconnect failed:" << reply.error().message();
        } else {
            qDebug() << "[Bluetooth] Disconnected!";
            if (m_connectedDevice) {
                m_connectedDevice->setConnected(false);
                m_connectedDevice = nullptr;
                emit connectedDeviceChanged();
                emit bluetoothMediaPlayerPathChanged(QString());
            }
        }
        watcher->deleteLater();
    });
}

void BluetoothController::clearDevices() {
    for (QObject* obj : std::as_const(m_pairedDevices)) {
        if (obj->parent() == this) {
            if (obj == m_connectedDevice) {
                qDebug() << "[BluetoothController] Clearing connected device:";
            }
            else obj->deleteLater();
        }
    }

    m_pairedDevices.clear();
    qDebug() << "Cleared paired devices";

    if (m_connectedDevice && m_connectedDevice->parent() == this) {
        m_connectedDevice->deleteLater();
    }
    m_connectedDevice = nullptr;

    emit pairedDevicesChanged();
    emit connectedDeviceChanged();
    qDebug() << "[BluetoothController] Clearing paired devices.";
}


void BluetoothController::updateConnectedDevice() {
    for (QObject* obj : std::as_const(m_pairedDevices)) {
        auto *dev = qobject_cast<BluetoothDevice*>(obj);
        if (dev && dev->connected()) {
            m_connectedDevice = dev;
            emit connectedDeviceChanged();
            break;
        }
    }
}

void BluetoothController::autoConnectIfAvailable() {
    for (QObject* obj : m_pairedDevices) {
        auto *dev = qobject_cast<BluetoothDevice*>(obj);
        if (dev && !dev->connected() && !m_connectedDevice) {
            connectToPairedDevice(dev->mac());
            qDebug() << "[BluetoothController] Auto-connecting to device:" << dev->name() << dev->mac();
            break;
        }
    }
}

bool BluetoothController::hasBluetoothAdapter() const {
    return !getAdapterPath().isEmpty();
}

QString BluetoothController::getAdapterPath() const {
    QDBusInterface manager("org.bluez", "/", "org.freedesktop.DBus.ObjectManager", QDBusConnection::systemBus());
    if (!manager.isValid()) {
        qWarning() << "[Bluetooth] BlueZ manager is not available on system bus.";
        return QString();
    }
    QDBusMessage reply = manager.call("GetManagedObjects");
    if (reply.type() != QDBusMessage::ReplyMessage || reply.arguments().isEmpty()) {
        qWarning() << "[Bluetooth] Failed to retrieve managed objects.";
        return QString();
    }
    auto managed = qdbus_cast<QMap<QDBusObjectPath, QMap<QString, QVariantMap>>>(reply.arguments().first());
    for (const auto &path : managed.keys()) {
        if (managed.value(path).contains("org.bluez.Adapter1")) {
            return path.path();
        }
    }
    return QString();
}

QString BluetoothController::macToDevicePath(const QString &mac) {
    QString formattedMac = mac;
    formattedMac.replace(":", "_");
    return getAdapterPath() + "/dev_" + formattedMac;
}

// --- Pairing agent methods, can remain as-is ---
void BluetoothController::startPairingAgent()
{
    makeDiscoverable(60);
    if (m_agentRegistered) return;

    // Step 1: Make discoverable as before
    // Step 2: Create and register the agent
    if (!m_agent)
        m_agent = new BluezAgent(this);

    // Connect agent prompt signal to your QML signal (old pairingPromptReceived, now agent-based)
    connect(m_agent, &BluezAgent::pairingPrompt, this, &BluetoothController::pairingPromptReceived, Qt::UniqueConnection);
    connect(m_agent, &BluezAgent::pairingAccepted, this, &::BluetoothController::onPairingAccepted, Qt::UniqueConnection);

    // Register agent on D-Bus
    bool ok = QDBusConnection::systemBus().registerObject("/my/agent", m_agent, QDBusConnection::ExportAllSlots);
    if (!ok) {
        qWarning() << "Could not register agent object on DBus!";
        return;
    }

    // Register agent with BlueZ
    QDBusInterface agentManager("org.bluez", "/org/bluez", "org.bluez.AgentManager1", QDBusConnection::systemBus());
    QDBusReply<void> reply = agentManager.call("RegisterAgent", QDBusObjectPath("/my/agent"), "KeyboardDisplay");
    if (!reply.isValid()) {
        qWarning() << "Failed to register agent:" << reply.error().message();
        return;
    }
    // Make it default (optional but recommended)
    agentManager.call("RequestDefaultAgent", QDBusObjectPath("/my/agent"));

    m_agentRegistered = true;
    qDebug() << "BlueZ Agent registered!";
}

void BluetoothController::stopPairingAgent()
{
    if (!m_agentRegistered) return;

    QDBusInterface agentManager("org.bluez", "/org/bluez", "org.bluez.AgentManager1", QDBusConnection::systemBus());
    agentManager.call("UnregisterAgent", QDBusObjectPath("/my/agent"));

    QDBusConnection::systemBus().unregisterObject("/my/agent");
    if (m_agent) {
        m_agent->deleteLater();
        m_agent = nullptr;
    }
    m_agentRegistered = false;
    qDebug() << "BlueZ Agent unregistered!";
}

void BluetoothController::confirmPairing(bool accept)
{
    if (m_agent) {
        qDebug() << "[BluetoothController] Forwarding pairing confirmation to agent:" << accept;
        m_agent->setPairingConfirmation(accept);
    } else {
        qWarning() << "[BluetoothController] Agent is null! Cannot confirm pairing.";
    }
}

void BluetoothController::onPairingAccepted(const QString &devicePath)
{
    if (devicePath.isEmpty()) {
        qWarning() << "[Bluetooth] onPairingAccepted: empty devicePath!";
        return;
    }
    trustDevice(devicePath);
    m_agent = nullptr;
}

void BluetoothController::trustDevice(const QString &devicePath)
{
    QDBusInterface dev("org.bluez", devicePath, "org.freedesktop.DBus.Properties", QDBusConnection::systemBus());
    QVariant arg = QVariant::fromValue(QDBusVariant(QVariant(true)));
    dev.call("Set", "org.bluez.Device1", "Trusted", arg);
}

QString BluetoothController::bluezPlayerPathForDevice(const QString &mac) const
{
    // Example input: "68:83:CB:79:36:4F"
    qDebug() << "[BluetoothController] Generating BlueZ player path for MAC:" << mac;
    QString adapter = "hci0"; // Use your actual adapter if you support multiple
    QString dev = mac;
    dev.replace(':', '_');
    return QString("/org/bluez/%1/dev_%2").arg(adapter, dev);
}


