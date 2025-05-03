#ifndef HVACHANDLER_H
#define HVACHANDLER_H

#include <QObject>

class HVACHandler : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int driverTemperature READ driverTemperature WRITE setDriverTemperature NOTIFY driverTemperatureChanged)
    Q_PROPERTY(int passengerTemperature READ passengerTemperature WRITE setPassengerTemperature NOTIFY passengerTemperatureChanged)
    Q_PROPERTY(bool ACEnabled READ ACEnabled WRITE setACEnabled NOTIFY ACEnabledChanged)
    Q_PROPERTY(bool syncEnabled READ syncEnabled WRITE setSyncEnabled NOTIFY syncEnabledChanged)
public:
    explicit HVACHandler(QObject *parent = nullptr);

    int driverTemperature() const;
    void setDriverTemperature(int newDriverTemperature);
    Q_INVOKABLE void increaseDriverTemperature();
    Q_INVOKABLE void decreaseDriverTemperature();

    int passengerTemperature() const;
    void setPassengerTemperature(int newPassengerTemperature);
    Q_INVOKABLE void increasePassengerTemperature();
    Q_INVOKABLE void decreasePassengerTemperature();

    bool ACEnabled() const;
    void setACEnabled(bool newACEnabled);

    bool syncEnabled() const;
    void setSyncEnabled(bool newSyncEnabled);

signals:
    void driverTemperatureChanged(int driverTemperature);
    void passengerTemperatureChanged(int passengerTemperature);
    void ACEnabledChanged(bool ACEnabled);
    void syncEnabledChanged(bool syncEnabled);

private:
    int m_driverTemperature;
    int m_passengerTemperature;
    bool m_ACEnabled;
    bool m_syncEnabled;
};

#endif // HVACHANDLER_H
