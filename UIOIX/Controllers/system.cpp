#include "system.h"

System::System(QObject *parent)
    : QObject(parent),
    m_carLocked(false),
    m_outdoorTemp(21),
    m_userName("Dragos"),
    m_batteryPercent(50),
    m_settings(this)
{
    m_tempUnit = m_settings.loadTempUnit();
    m_speedUnit = m_settings.loadSpeedUnit();
    m_interiorColor = m_settings.loadInteriorColor();
    m_speedWarning = m_settings.loadSpeedWarning();
}

bool System::carLocked() const
{
    return m_carLocked;
}

void System::setCarLocked(bool newCarLocked)
{
    if (m_carLocked == newCarLocked)
        return;
    m_carLocked = newCarLocked;
    emit carLockedChanged(m_carLocked);
}

int System::outdoorTemp() const
{
    return convertCelsiusToFahrenheit(m_outdoorTemp);
}

void System::setOutdoorTemp(int newOutdoorTemp)
{
    if (m_outdoorTemp == newOutdoorTemp)
        return;
    m_outdoorTemp = newOutdoorTemp;
    emit outdoorTempChanged(m_outdoorTemp);
}

QString System::userName() const
{
    return m_userName;
}

void System::setUserName(const QString &newUserName)
{
    if (m_userName == newUserName)
        return;
    m_userName = newUserName;
    emit userNameChanged(m_userName);
}

int System::batteryPercent() const
{
    return m_batteryPercent;
}

void System::setBatteryPercent(int newBatteryPercent)
{
    if (m_batteryPercent == newBatteryPercent)
        return;
    m_batteryPercent = newBatteryPercent;
    emit batteryPercentChanged(m_batteryPercent);
}


QString System::tempUnit() const
{
    return m_tempUnit;
}

void System::setTempUnit(const QString &newTempUnit)
{
    if (m_tempUnit == newTempUnit)
        return;
    m_tempUnit = newTempUnit;
    m_settings.saveTempUnit(m_tempUnit);
    emit tempUnitChanged(m_tempUnit);
    emit outdoorTempChanged(outdoorTemp());  // ⚠️ Added to reflect updated value
}


QString System::speedUnit() const {
    return m_speedUnit;
}

void System::setSpeedUnit(const QString &newSpeedUnit)
{
    if (m_speedUnit == newSpeedUnit)
        return;
    m_speedUnit = newSpeedUnit;
    m_distanceUnit = (newSpeedUnit == "mph") ? "mi" : "km";
    m_settings.saveSpeedUnit(m_speedUnit);
    emit speedUnitChanged(m_speedUnit);
    emit distanceUnitChanged();
}

QColor System::interiorColor() const {
    return m_interiorColor;
}

void System::setInteriorColor(const QColor &newColor)
{
    if (m_interiorColor == newColor)
        return;
    m_interiorColor = newColor;
    m_settings.saveInteriorColor(m_interiorColor);
    emit interiorColorChanged(m_interiorColor);
}

int System::speedWarning() const {
    return m_speedWarning;
}

void System::setSpeedWarning(int newWarning)
{
    if (m_speedWarning == newWarning)
        return;
    m_speedWarning = newWarning;
    m_settings.saveSpeedWarning(m_speedWarning);
    emit speedWarningChanged(m_speedWarning);
}

int System::convertCelsiusToFahrenheit(int temperature) const {
    if (m_tempUnit == "F°") {
        return static_cast<int>(temperature * 1.8 + 32);
    }
    return temperature;
}

int System::convertKilometersToMiles(int kilometers) const {
    if (m_distanceUnit == "mi") {
        return static_cast<int>(kilometers * 0.621371);
    }
    return kilometers;
}

int System::convertKMHToMPH(int kmh) const {
    if (m_speedUnit == "mph") {
        return static_cast<int>(kmh * 0.621371);
    }
    return kmh;
}

QString System::distanceUnit() const
{
    return m_distanceUnit;
}

void System::setDistanceUnit(const QString &newDistanceUnit)
{
    if (m_distanceUnit == newDistanceUnit)
        return;
    m_distanceUnit = newDistanceUnit;
    m_speedUnit = (newDistanceUnit == "mi") ? "mph" : "km/h";
    emit distanceUnitChanged();
    emit speedUnitChanged(m_speedUnit);
}
