#ifndef HVACHANDLER_H
#define HVACHANDLER_H

#include <QObject>
#include <Controllers/zonecontroller.h>
#include "cancontroller.h"


class HVACHandler : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool airFromOutside READ airFromOutside WRITE setAirFromOutside NOTIFY airFromOutsideChanged)
    Q_PROPERTY(bool rearHeater READ rearHeater WRITE setRearHeater NOTIFY rearHeaterChanged)
    Q_PROPERTY(bool maxAC READ maxAC WRITE setMaxAC NOTIFY maxACChanged)
    Q_PROPERTY(bool maxHeat READ maxHeat WRITE setMaxHeat NOTIFY maxHeatChanged)
    Q_PROPERTY(bool ACEnabled READ ACEnabled WRITE setACEnabled NOTIFY ACEnabledChanged)
    Q_PROPERTY(bool syncEnabled READ syncEnabled WRITE setSyncEnabled NOTIFY syncEnabledChanged)
    Q_PROPERTY(bool speedFan READ speedFan WRITE setSpeedFan NOTIFY speedFanChanged)

    Q_PROPERTY(ZoneController *driverZone READ driverZone CONSTANT)
    Q_PROPERTY(ZoneController *passengerZone READ passengerZone CONSTANT)
public:
    explicit HVACHandler(QObject *parent = nullptr);

    bool ACEnabled() const;
    void setACEnabled(bool newACEnabled);

    bool syncEnabled() const;
    void setSyncEnabled(bool newSyncEnabled);

    bool airFromOutside() const;
    void setAirFromOutside(bool newAirFromOutside);

    bool rearHeater() const;
    void setRearHeater(bool newRearHeater);

    bool maxAC() const;
    void setMaxAC(bool newMaxAC);

    bool maxHeat() const;
    void setMaxHeat(bool newMaxHeat);

    ZoneController *driverZone() const;
    ZoneController *passengerZone() const;

    bool speedFan() const;
    void setSpeedFan(bool newSpeedFan);

signals:
    void ACEnabledChanged(bool ACEnabled);
    void syncEnabledChanged(bool syncEnabled);

    void airFromOutsideChanged();

    void rearHeaterChanged();

    void maxACChanged();

    void maxHeatChanged();

    void speedFanChanged();

private:
    CanController *m_can;
    int m_driverTemperature;
    int m_passengerTemperature;
    bool m_ACEnabled;
    bool m_syncEnabled;
    bool m_airFromOutside;
    bool m_rearHeater;
    bool m_maxAC;
    bool m_maxHeat;
    ZoneController* m_driverZone ;
    ZoneController* m_passengerZone ;
    bool m_speedFan;
};

#endif // HVACHANDLER_H
