#include "zonecontroller.h"

ZoneController::ZoneController(QObject *parent)
    : QObject(parent),
      m_autoEnabled(false),
      m_temperature(21),
      m_seatHeating(0)
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
}

void ZoneController::increaseTemperature(const int &value)
{
    setTemperature(std::clamp(m_temperature + value, 16, 28));
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
}
