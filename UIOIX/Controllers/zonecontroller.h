#ifndef ZONECONTROLLER_H
#define ZONECONTROLLER_H

#include <QObject>
#include "cancontroller.h"

class ZoneController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool autoEnabled READ autoEnabled WRITE setAutoEnabled NOTIFY autoEnabledChanged)
    Q_PROPERTY(int temperature READ temperature WRITE setTemperature NOTIFY temperatureChanged)
    Q_PROPERTY(int seatHeating READ seatHeating WRITE setSeatHeating NOTIFY seatHeatingChanged)
    Q_PROPERTY(int airZoneMask READ airZoneMask WRITE setAirZoneMask NOTIFY airZoneMaskChanged)
public:
    enum AirZone {
        Head = 1 << 0,
        Body = 1 << 1,
        Legs = 1 << 2
    };
    Q_ENUM(AirZone)

    explicit ZoneController(CanController *canCtrl, const QString &role, QObject *parent = nullptr);

    bool autoEnabled() const;
    void setAutoEnabled(bool newAutoEnabled);

    Q_INVOKABLE void increaseTemperature(const int &value);
    Q_INVOKABLE void increaseSeatHeating();
    Q_INVOKABLE bool isZoneActive(AirZone zone) const;
    Q_INVOKABLE void toggleZone(AirZone zone);

    int temperature() const;
    void setTemperature(int newTemperature);

    int seatHeating() const;
    void setSeatHeating(int newSeatHeating);

    int airZoneMask() const;
    void setAirZoneMask(int newAirZoneMask);

signals:
    void autoEnabledChanged();
    void temperatureChanged();

    void seatHeatingChanged();

    void airZoneMaskChanged();

private:
    bool m_autoEnabled;
    int m_temperature;
    int m_seatHeating;
    int m_airZoneMask;
    CanController *m_can = nullptr;
    QString m_role;
};

#endif // ZONECONTROLLER_H
