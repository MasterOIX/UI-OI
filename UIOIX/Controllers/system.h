#ifndef SYSTEM_H
#define SYSTEM_H

#include <QObject>

class System : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool carLocked READ carLocked WRITE setCarLocked NOTIFY carLockedChanged)
    Q_PROPERTY(int outdoorTemp READ outdoorTemp WRITE setOutdoorTemp NOTIFY outdoorTempChanged)
    Q_PROPERTY(QString userName READ userName WRITE setUserName NOTIFY userNameChanged)
    Q_PROPERTY(int batteryPercent READ batteryPercent WRITE setBatteryPercent NOTIFY batteryPercentChanged)


public:
    explicit System(QObject *parent = nullptr);

    bool carLocked() const;
    void setCarLocked(bool newCarLocked);

    int outdoorTemp() const;
    void setOutdoorTemp(int newOutdoorTemp);

    QString userName() const;
    void setUserName(const QString &newUserName);

    int batteryPercent() const;
    void setBatteryPercent(int newBatteryPercent);

signals:

    void carLockedChanged(bool carLocked);
    void outdoorTempChanged(int outdoorTemp);
    void userNameChanged(QString userName);
    void batteryPercentChanged(int batteryPercent);

private:
    bool m_carLocked;
    int m_outdoorTemp;
    QString m_userName;
    int m_batteryPercent;
};

#endif // SYSTEM_H
