#ifndef SYSTEM_H
#define SYSTEM_H

#include <QObject>
#include <QString>

class System : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool carLocked READ carLocked WRITE setCarLocked NOTIFY carLockedChanged)
    Q_PROPERTY(int outdoorTemp READ outdoorTemp WRITE setOutdoorTemp NOTIFY outdoorTempChanged)
    Q_PROPERTY(QString userName READ userName WRITE setUserName NOTIFY userNameChanged)
    Q_PROPERTY(int batteryPercent READ batteryPercent WRITE setBatteryPercent NOTIFY batteryPercentChanged)
    Q_PROPERTY(QString tempUnit READ tempUnit WRITE setTempUnit NOTIFY tempUnitChanged)

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

    QString tempUnit() const;
    void setTempUnit(const QString &newTempUnit);

signals:

    void carLockedChanged(bool carLocked);
    void outdoorTempChanged(int outdoorTemp);
    void userNameChanged(QString userName);
    void batteryPercentChanged(int batteryPercent);
    void tempUnitChanged(QString tempUnit);

private:
    bool m_carLocked;
    int m_outdoorTemp;
    QString m_userName;
    int m_batteryPercent;
    QString m_tempUnit;
};

#endif // SYSTEM_H
