#include "wificontroller.h"
#include <QProcess>
#include <QStringList>
#include <QDebug>

WiFiController::WiFiController(QObject *parent)
    : QObject(parent), m_wifiEnabled(true), m_currentNetwork(nullptr), m_connectionFailed(false)
{
}

bool WiFiController::wifiEnabled() const
{
    return m_wifiEnabled;
}

void WiFiController::setWifiEnabled(bool newWifiEnabled)
{
    if (m_wifiEnabled == newWifiEnabled)
        return;
    m_wifiEnabled = newWifiEnabled;
    emit wifiEnabledChanged();
}

QList<QObject *> WiFiController::availableNetworks() const
{
    return m_availableNetworks;
}

QObject *WiFiController::currentNetwork() const
{
    return m_currentNetwork;
}

void WiFiController::scanNetworks()
{
    // Step 1: Get currently connected SSID
    QProcess checkProcess;
    checkProcess.start("nmcli", {"-t", "-f", "active,ssid", "dev", "wifi"});
    checkProcess.waitForFinished();

    QString activeOutput = checkProcess.readAllStandardOutput();
    QString currentSsid;

    QStringList activeLines = activeOutput.split('\n', Qt::SkipEmptyParts);
    for (const QString &line : activeLines) {
        QStringList parts = line.split(':');
        if (parts.size() == 2 && parts[0] == "yes") {
            currentSsid = parts[1];
            break;
        }
    }

    // Step 2: Scan for available networks
    QProcess* process = new QProcess(this);

    connect(process,
            static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
            this,
            [=](int, QProcess::ExitStatus) {
                QString output = process->readAllStandardOutput();
                QStringList lines = output.split('\n', Qt::SkipEmptyParts);

                qDeleteAll(m_availableNetworks);
                m_availableNetworks.clear();
                m_currentNetwork = nullptr;

                for (const QString &line : std::as_const(lines)) {
                    QStringList parts = line.split(':');
                    if (parts.size() >= 3) {
                        WiFiNetwork* network = new WiFiNetwork(nullptr);
                        network->setName(parts[0]);
                        network->setSignal(parts[1].toInt());
                        network->setSecurity(parts[2]);
                        m_availableNetworks.append(network);

                        // ✅ Match with current SSID
                        if (!currentSsid.isEmpty() && parts[0] == currentSsid) {
                            m_currentNetwork = network;
                            emit currentNetworkChanged();
                        }
                    }
                }

                emit availableNetworksChanged();
                process->deleteLater();
            });

    process->start("nmcli", {"-t", "-f", "SSID,SIGNAL,SECURITY", "device", "wifi", "list"});
}

void WiFiController::connectToNetwork(QObject* networkObj)
{
    auto network = qobject_cast<WiFiNetwork*>(networkObj);
    if (!network) {
        qWarning() << "[WiFiController] Invalid network object.";
        return;
    }

    QString ssid = network->name();
    QString password = network->password();
    QStringList connectArgs;

    if (password.isEmpty()) {
        connectArgs << "device" << "wifi" << "connect" << ssid;
    } else {
        connectArgs << "device" << "wifi" << "connect" << ssid << "password" << password;
    }

    QProcess* connectProcess = new QProcess(this);
    connect(connectProcess,
            static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
            this,
            [=](int, QProcess::ExitStatus) {
                QString output = connectProcess->readAllStandardOutput();
                QString error = connectProcess->readAllStandardError();

                if (output.contains("successfully activated") || error.contains("already active")) {
                    m_currentNetwork = network;
                    emit currentNetworkChanged();
                    qDebug() << "[WiFiController] Connected to:" << ssid;

                    // ✅ Step 2: Set autoconnect ON
                    QProcess* autoConnectProcess = new QProcess(this);
                    autoConnectProcess->start("nmcli", {"connection", "modify", ssid, "connection.autoconnect", "yes"});
                    autoConnectProcess->deleteLater();

                    setConnectionFailed(false);
                } else {
                    qWarning() << "[WiFiController] Failed to connect:" << ssid;
                    qDebug() << "stderr:" << error;
                    qDebug() << "stdout:" << output;
                    setConnectionFailed(true);
                }

                connectProcess->deleteLater();
            });

    connectProcess->start("nmcli", connectArgs);
}

bool WiFiController::isKnownAndTrusted(const QString &ssid) const
{
    QProcess process;
    process.start("nmcli", {"-t", "-f", "name,autoconnect", "connection", "show"});
    process.waitForFinished();

    QString output = process.readAllStandardOutput();
    QStringList lines = output.split('\n', Qt::SkipEmptyParts);

    for (const QString &line : lines) {
        QStringList parts = line.split(':');
        if (parts.size() == 2 && parts[0] == ssid && parts[1] == "yes") {
            return true;
        }
    }
    return false;
}


void WiFiController::disconnectFromCurrentNetwork()
{
    if (!m_currentNetwork) {
        qWarning() << "[WiFiController] No network currently connected.";
        return;
    }

    QProcess process;
    QString command = "nmcli connection show --active";
    process.start("bash", {"-c", command});
    process.waitForFinished();

    QString output = process.readAllStandardOutput();
    QString currentSSID = m_currentNetwork->name();
    QString connectionName;

    // Try to find matching connection name (some distros auto-save connection names)
    QStringList lines = output.split('\n', Qt::SkipEmptyParts);
    for (const QString &line : std::as_const(lines)) {
        if (line.contains(currentSSID)) {
            connectionName = line.split(" ").first().trimmed(); // first column
            break;
        }
    }

    if (!connectionName.isEmpty()) {
        QString disconnectCmd = QString("nmcli connection down \"%1\"").arg(connectionName);
        qDebug() << "[WiFiController] Disconnecting with:" << disconnectCmd;

        process.start("bash", {"-c", disconnectCmd});
        process.waitForFinished();

        QString result = process.readAllStandardOutput();
        QString err = process.readAllStandardError();
        qDebug() << "[WiFiController] Output:" << result << err;

        m_currentNetwork = nullptr;
        emit currentNetworkChanged();
    } else {
        qWarning() << "[WiFiController] Could not find matching active connection to disconnect.";
    }
}

bool WiFiController::connectionFailed() const
{
    return m_connectionFailed;
}

void WiFiController::setConnectionFailed(bool newConnectionFailed)
{
    if (m_connectionFailed == newConnectionFailed)
        return;
    m_connectionFailed = newConnectionFailed;
    emit connectionFailedChanged();
}
