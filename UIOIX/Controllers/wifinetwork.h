#ifndef WIFINETWORK_H
#define WIFINETWORK_H

#include <QObject>

class WiFiNetwork : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged FINAL)
    Q_PROPERTY(int signal READ signal WRITE setSignal NOTIFY signalChanged FINAL)
    Q_PROPERTY(QString security READ security WRITE setSecurity NOTIFY securityChanged FINAL)
    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged FINAL)

public:
    explicit WiFiNetwork(QObject *parent = nullptr);

    QString name() const;
    void setName(const QString &newName);

    int signal() const;
    void setSignal(int newSignal);

    QString security() const;
    void setSecurity(const QString &newSecurity);

    QString password() const;
    void setPassword(const QString &newPassword);

signals:
    void nameChanged();
    void signalChanged();
    void securityChanged();
    void passwordChanged();

private:
    QString m_name;
    int m_signal;
    QString m_security;
    QString m_password;
};

#endif // WIFINETWORK_H
