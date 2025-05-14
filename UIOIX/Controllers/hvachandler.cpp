#include "hvachandler.h"

HVACHandler::HVACHandler(QObject *parent)
    : QObject(parent),
      m_ACEnabled(false),
      m_syncEnabled(false),
      m_airFromOutside(false),
      m_rearHeater(false),
      m_maxAC(false),
      m_maxHeat(false),
      m_driverZone(new ZoneController(this)),
      m_passengerZone(new ZoneController(this))
{

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
}

ZoneController* HVACHandler::driverZone() const {
    return m_driverZone;
}

ZoneController* HVACHandler::passengerZone() const {
    return m_passengerZone;
}
