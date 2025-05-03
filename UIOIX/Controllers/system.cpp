#include "system.h"

System::System(QObject *parent)
    : QObject(parent),
      m_carLocked(false),
      m_outdoorTemp(21)
{

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
    return m_outdoorTemp;
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
