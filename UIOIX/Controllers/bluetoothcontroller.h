#ifndef BLUETOOTHCONTROLLER_H
#define BLUETOOTHCONTROLLER_H

#include <QObject>
#include <QList>
#include <QTimer>
#include <QDBusInterface>
#include <QDBusObjectPath>
#include <QDBusVariant>
#include <QDBusPendingCallWatcher>
#include <QProcess>
#include <functional>
#include "bluezagent.h"

class BluetoothDevice : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString name READ name CONSTANT)
    Q_PROPERTY(QString mac READ mac CONSTANT)
    Q_PROPERTY(bool connected READ connected WRITE setConnected NOTIFY connectedChanged)

public:
    BluetoothDevice(const QString &name, const QString &mac, bool connected, QObject *parent = nullptr)
        : QObject(parent), m_name(name), m_mac(mac), m_connected(connected) {}

    QString name() const { return m_name; }
    QString mac() const { return m_mac; }

    bool connected() const { return m_connected; }
    void setConnected(bool connected) {
        if (m_connected != connected) {
            m_connected = connected;
            emit connectedChanged();
        }
    }

signals:
    void connectedChanged();

private:
    QString m_name;
    QString m_mac;
    bool m_connected;
};

class BluetoothController : public QObject
{
    Q_OBJECT
    // These Q_PROPERTY are for QML binding, use the *Async() methods for UI updates if needed!
    Q_PROPERTY(bool bluetoothEnabled READ bluetoothEnabled NOTIFY bluetoothEnabledChanged)
    Q_PROPERTY(bool pairable READ pairable NOTIFY pairableChanged)
    Q_PROPERTY(QString pairingCode READ pairingCode NOTIFY pairingCodeChanged)
    Q_PROPERTY(QList<QObject*> pairedDevices READ pairedDevices NOTIFY pairedDevicesChanged)
    Q_PROPERTY(QObject* connectedDevice READ connectedDevice NOTIFY connectedDeviceChanged)
    Q_PROPERTY(bool bluetoothOperationPending READ bluetoothOperationPending NOTIFY bluetoothOperationPendingChanged)


public:
    explicit BluetoothController(QObject *parent = nullptr);

    // Deprecated: direct getter, prefer async call!
    bool bluetoothEnabled() const { return m_bluetoothEnabled; }
    bool pairable() const { return m_pairable; }
    QString pairingCode() const { return m_pairingCode; }
    QList<QObject*> pairedDevices() const { return m_pairedDevices; }
    QObject* connectedDevice() const { return m_connectedDevice; }
    bool bluetoothOperationPending() const { return m_btPending; }

    // Async methods for D-Bus interactions
    Q_INVOKABLE void bluetoothEnabledAsync(std::function<void(bool)> callback) ;
    Q_INVOKABLE void setBluetoothEnabled(bool enabled); // Remains void, emits signal on completion

    Q_INVOKABLE void connectToPairedDevice(const QString &mac);
    Q_INVOKABLE void disconnectFromDevice();
    Q_INVOKABLE void removePairedDevice(const QString &mac);
    Q_INVOKABLE void makeDiscoverable(int seconds);
    Q_INVOKABLE void startPairingAgent();
    Q_INVOKABLE void stopPairingAgent();
    Q_INVOKABLE void confirmPairing(bool accept);


signals:
    void bluetoothEnabledChanged();
    void pairableChanged();
    void pairingCodeChanged();
    void pairedDevicesChanged();
    void connectedDeviceChanged();
    void bluetoothOperationPendingChanged(bool changed);
    Q_SIGNAL void pairingPromptReceived(const QString &prompt);
    void bluetoothMediaPlayerPathChanged(const QString &mprisPath);

private:
    void clearDevices();
    void scanPairedDevices();
    void updateConnectedDevice();
    void autoConnectIfAvailable();
    bool hasBluetoothAdapter() const;
    QString getAdapterPath() const;
    void refreshDeviceProperties(const QString &devicePath);
    void connectToDevicePath(const QString &devicePath);
    QString macToDevicePath(const QString &mac);
    void onPairingAccepted(const QString &devicePath);
    void trustDevice(const QString &devicePath);
    QString bluezPlayerPathForDevice(const QString &mac) const;

    bool m_bluetoothEnabled = false;
    bool m_pairable = false;
    QString m_pairingCode;

    QList<QObject*> m_pairedDevices;
    BluetoothDevice *m_connectedDevice = nullptr;

    QTimer m_discoverableTimer;
    bool m_isDiscoverable = false;

    bool m_btPending;

    QProcess *m_agentProcess = nullptr;
    bool m_bluetoothOperationPending;

    BluezAgent *m_agent = nullptr;
    bool m_agentRegistered = false;
};

#endif // BLUETOOTHCONTROLLER_H
