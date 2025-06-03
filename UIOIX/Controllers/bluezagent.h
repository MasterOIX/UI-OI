#pragma once
#include <QObject>
#include <QtDBus>
#include <QEventLoop>
#include <optional>

class BluezAgent : public QObject {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.bluez.Agent1")
public:
    explicit BluezAgent(QObject *parent = nullptr) : QObject(parent) {}

public slots:
    void Release() {}
    void Cancel() {}

    QString RequestPinCode(const QDBusObjectPath &device) {
        qDebug() << "RequestPinCode called!";
        emit pairingPrompt("Enter PIN code for device " + device.path());
        // For demo, just return "0000" synchronously
        return "0000";
    }

    void RequestConfirmation(const QDBusObjectPath &device, uint passkey) {
        qDebug() << "RequestConfirmation called!";
        m_lastConfirmation = std::nullopt;
        QString devName = getDeviceName(device);
        if (devName.isEmpty())
            devName = device.path();
        QEventLoop loop;
        m_confirmationLoop = &loop;
        emit pairingPrompt(
            QString("Confirm pairing for device \"%1\" with passkey %2")
                .arg(devName)
                .arg(passkey)
            );
        loop.exec();
        if (m_lastConfirmation.value_or(false)) {
            emit pairingAccepted(device.path());

            // OPTIONAL: Deregister from D-Bus (recommended)
            QDBusInterface agentManager("org.bluez", "/org/bluez", "org.bluez.AgentManager1", QDBusConnection::systemBus());
            agentManager.call("UnregisterAgent", QDBusObjectPath("/my/agent"));

            // Delete this agent object after a short delay, or using deleteLater()
            QTimer::singleShot(100, this, [this]() {
                this->deleteLater();
            });

            return;
        } else {
            throw QDBusError(QDBusError::AccessDenied, "User rejected pairing");
        }

    }

    QString getDeviceName(const QDBusObjectPath& devicePath) {
        QDBusInterface deviceIface(
            "org.bluez", devicePath.path(),
            "org.freedesktop.DBus.Properties",
            QDBusConnection::systemBus()
            );

        QDBusReply<QVariant> reply = deviceIface.call(
            "Get", "org.bluez.Device1", "Name"
            );
        if (reply.isValid())
            return reply.value().toString();
        return QString(); // fallback: unknown
    }

    // This slot should be called from your controller/QML when user clicks yes/no
    Q_INVOKABLE void setPairingConfirmation(bool accept) {
        m_lastConfirmation = accept;
        if (m_confirmationLoop) {
            m_confirmationLoop->quit();
            m_confirmationLoop = nullptr;
        }
    }

signals:
    void pairingPrompt(const QString &prompt);
    void pairingAccepted(const QString &devicePath); // add this to signals

private:
    std::optional<bool> m_lastConfirmation;
    QEventLoop *m_confirmationLoop = nullptr;
};
