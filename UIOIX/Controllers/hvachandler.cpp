#include "hvachandler.h"

HVACHandler::HVACHandler(QObject *parent)
    : QObject(parent),
      m_driverTemperature(21),
      m_passengerTemperature(21),
      m_ACEnabled(false),
      m_syncEnabled(false)
{}

int HVACHandler::driverTemperature() const
{
    return m_driverTemperature;
}

void HVACHandler::setDriverTemperature(int newDriverTemperature)
{
    if (m_driverTemperature == newDriverTemperature)
        return;
    m_driverTemperature = newDriverTemperature;
    emit driverTemperatureChanged(m_driverTemperature);
}

void HVACHandler::increaseDriverTemperature()
{
    if (m_syncEnabled) {
        setPassengerTemperature(m_passengerTemperature + 1);
    }
    setDriverTemperature(m_driverTemperature + 1);
}

void HVACHandler::decreaseDriverTemperature()
{
    if (m_syncEnabled) {
        setPassengerTemperature(m_passengerTemperature - 1);
    }
    setDriverTemperature(m_driverTemperature - 1);
}

int HVACHandler::passengerTemperature() const
{
    return m_passengerTemperature;
}

void HVACHandler::setPassengerTemperature(int newPassengerTemperature)
{
    if (m_passengerTemperature == newPassengerTemperature)
        return;
    m_passengerTemperature = newPassengerTemperature;
    emit passengerTemperatureChanged(m_passengerTemperature);
}

void HVACHandler::increasePassengerTemperature()
{
    if (m_syncEnabled) {
        setSyncEnabled(false);
    }
    setPassengerTemperature(m_passengerTemperature + 1);
}

void HVACHandler::decreasePassengerTemperature()
{
    if (m_syncEnabled) {
        setSyncEnabled(false);
    }
    setPassengerTemperature(m_passengerTemperature - 1);
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
        setPassengerTemperature(m_driverTemperature);
    }
    emit syncEnabledChanged(m_syncEnabled);
}
