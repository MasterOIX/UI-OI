#include "zonecontroller.h"

ZoneController::ZoneController(CanController *canCtrl, const QString &role, QObject *parent)
    : QObject(parent),
    m_autoEnabled(false),
    m_temperature(21),
    m_seatHeating(0),
    m_airZoneMask(0),
    m_can(canCtrl),
    m_role(role)
{}


bool ZoneController::autoEnabled() const
{
    return m_autoEnabled;
}

void ZoneController::setAutoEnabled(bool newAutoEnabled)
{
    if (m_autoEnabled == newAutoEnabled)
        return;
    m_autoEnabled = newAutoEnabled;
    emit autoEnabledChanged();

    if (m_can)
        m_can->canSend(QString("%1:AUTO=%2").arg(m_role).arg(m_autoEnabled ? 1 : 0));
}

void ZoneController::increaseTemperature(const int &value)
{
    setTemperature(std::clamp(m_temperature + value, 16, 28));
}

void ZoneController::increaseSeatHeating()
{
    if (m_seatHeating < 3)
        setSeatHeating(m_seatHeating + 1);
    else
        setSeatHeating(0);
}

bool ZoneController::isZoneActive(AirZone zone) const
{
    return (m_airZoneMask & zone);
}

void ZoneController::toggleZone(AirZone zone)
{
    setAirZoneMask(m_airZoneMask ^ zone);
}

int ZoneController::temperature() const
{
    return m_temperature;
}

void ZoneController::setTemperature(int newTemperature)
{
    if (m_temperature == newTemperature)
        return;
    m_temperature = newTemperature;
    emit temperatureChanged();

    if (m_can)
        m_can->canSend(QString("%1:TEMP=%2").arg(m_role).arg(m_temperature));
}

int ZoneController::seatHeating() const
{
    return m_seatHeating;
}

void ZoneController::setSeatHeating(int newSeatHeating)
{
    if (m_seatHeating == newSeatHeating)
        return;
    m_seatHeating = newSeatHeating;
    emit seatHeatingChanged();

    if (m_can)
        m_can->canSend(QString("%1:HEAT=%2").arg(m_role).arg(newSeatHeating));
}

int ZoneController::airZoneMask() const
{
    return m_airZoneMask;
}

void ZoneController::setAirZoneMask(int newAirZoneMask)
{
    if (m_airZoneMask == newAirZoneMask)
        return;
    m_airZoneMask = newAirZoneMask;
    emit airZoneMaskChanged();

    if (m_can)
        m_can->canSend(QString("%1:ZONES=%2").arg(m_role).arg(m_airZoneMask));
}
