#include "hvachandler.h"

HVACHandler::HVACHandler(QObject *parent)
    : QObject(parent),
    m_can(new CanController(this)),
    m_ACEnabled(false), // Initialize first
    m_syncEnabled(false),
    m_airFromOutside(false),
    m_rearHeater(false),
    m_maxAC(false),
    m_maxHeat(false)
{
    m_can->initialize("can0");
    m_driverZone = new ZoneController(m_can, "DRIVER", this);
    m_passengerZone = new ZoneController(m_can, "PASSENGER", this);
}

bool HVACHandler::ACEnabled() const
{
    return m_ACEnabled;
}

void HVACHandler::setACEnabled(bool newACEnabled)
{
    if (m_ACEnabled == newACEnabled)
        return;
    m_ACEnabled = newACEnabled;
    emit ACEnabledChanged(m_ACEnabled);

    m_can->canSend(QString("AC:%1").arg(m_ACEnabled));
}

bool HVACHandler::syncEnabled() const
{
    return m_syncEnabled;
}

void HVACHandler::setSyncEnabled(bool newSyncEnabled)
{
    if (m_syncEnabled == newSyncEnabled)
        return;
    m_syncEnabled = newSyncEnabled;
    if (m_syncEnabled) {
        m_passengerZone->setTemperature(m_driverZone->temperature());
    }
    emit syncEnabledChanged(m_syncEnabled);

    m_can->canSend(QString("SYNC:%1").arg(m_syncEnabled));
}

bool HVACHandler::airFromOutside() const
{
    return m_airFromOutside;
}

void HVACHandler::setAirFromOutside(bool newAirFromOutside)
{
    if (m_airFromOutside == newAirFromOutside)
        return;
    m_airFromOutside = newAirFromOutside;
    emit airFromOutsideChanged();

    m_can->canSend(QString("AIR_OUT:%1").arg(m_airFromOutside));
}

bool HVACHandler::rearHeater() const
{
    return m_rearHeater;
}

void HVACHandler::setRearHeater(bool newRearHeater)
{
    if (m_rearHeater == newRearHeater)
        return;
    m_rearHeater = newRearHeater;
    emit rearHeaterChanged();

    m_can->canSend(QString("REAR_HEATER:%1").arg(m_rearHeater));
}

bool HVACHandler::maxAC() const
{
    return m_maxAC;
}

void HVACHandler::setMaxAC(bool newMaxAC)
{
    if (m_maxAC == newMaxAC)
        return;
    m_maxAC = newMaxAC;
    emit maxACChanged();

    m_can->canSend(QString("MAXAC:%1").arg(m_maxAC));
}

bool HVACHandler::maxHeat() const
{
    return m_maxHeat;
}

void HVACHandler::setMaxHeat(bool newMaxHeat)
{
    if (m_maxHeat == newMaxHeat)
        return;
    m_maxHeat = newMaxHeat;
    emit maxHeatChanged();

    m_can->canSend(QString("MAX_HEAT:%1").arg(m_maxHeat));
}

ZoneController* HVACHandler::driverZone() const {
    return m_driverZone;
}

ZoneController* HVACHandler::passengerZone() const {
    return m_passengerZone;
}

bool HVACHandler::speedFan() const
{
    return m_speedFan;
}

void HVACHandler::setSpeedFan(bool newSpeedFan)
{
    if (m_speedFan == newSpeedFan)
        return;
    m_speedFan = newSpeedFan;
    emit speedFanChanged();

    m_can->canSend(QString("SPEED_FAN:%1").arg(m_speedFan));
}
