#ifndef WIFICONTROLLER_H
#define WIFICONTROLLER_H

#include <QObject>
#include <QList>
#include <QPointer>
#include "wifinetwork.h"

class WiFiController : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool wifiEnabled READ wifiEnabled WRITE setWifiEnabled NOTIFY wifiEnabledChanged FINAL)
    Q_PROPERTY(QList<QObject*> availableNetworks READ availableNetworks NOTIFY availableNetworksChanged FINAL)
    Q_PROPERTY(QObject* currentNetwork READ currentNetwork NOTIFY currentNetworkChanged FINAL)
    Q_PROPERTY(bool connectionFailed READ connectionFailed WRITE setConnectionFailed NOTIFY connectionFailedChanged FINAL)

public:
    explicit WiFiController(QObject *parent = nullptr);

    Q_INVOKABLE void scanNetworks();
    Q_INVOKABLE void connectToNetwork(QObject* network);
    Q_INVOKABLE void disconnectFromCurrentNetwork();
    Q_INVOKABLE bool isKnownAndTrusted(const QString &ssid) const;

    bool wifiEnabled() const;
    void setWifiEnabled(bool newWifiEnabled);

    QList<QObject *> availableNetworks() const;

    QObject *currentNetwork() const;

    bool connectionFailed() const;
    void setConnectionFailed(bool newConnectionFailed);

signals:

    void wifiEnabledChanged();
    void availableNetworksChanged();
    void currentNetworkChanged();

    void connectionFailedChanged();

private:
    bool m_wifiEnabled;
    QList<QObject*> m_availableNetworks;
    QPointer<WiFiNetwork> m_currentNetwork;
    bool m_connectionFailed;
};

#endif // WIFICONTROLLER_H
