#ifndef SYSTEM_H
#define SYSTEM_H

#include "Controllers/settingsmanager.h"
#include <QObject>
#include <QString>
#include <QColor>

class System : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool carLocked READ carLocked WRITE setCarLocked NOTIFY carLockedChanged)
    Q_PROPERTY(int outdoorTemp READ outdoorTemp WRITE setOutdoorTemp NOTIFY outdoorTempChanged)
    Q_PROPERTY(QString userName READ userName WRITE setUserName NOTIFY userNameChanged)
    Q_PROPERTY(int batteryPercent READ batteryPercent WRITE setBatteryPercent NOTIFY batteryPercentChanged)
    Q_PROPERTY(QString tempUnit READ tempUnit WRITE setTempUnit NOTIFY tempUnitChanged)
    Q_PROPERTY(QString speedUnit READ speedUnit WRITE setSpeedUnit NOTIFY speedUnitChanged)
    Q_PROPERTY(QString distanceUnit READ distanceUnit WRITE setDistanceUnit NOTIFY distanceUnitChanged)
    Q_PROPERTY(QColor interiorColor READ interiorColor WRITE setInteriorColor NOTIFY interiorColorChanged)
    Q_PROPERTY(int speedWarning READ speedWarning WRITE setSpeedWarning NOTIFY speedWarningChanged)

public:
    explicit System(QObject *parent = nullptr);

    bool carLocked() const;
    void setCarLocked(bool newCarLocked);

    int outdoorTemp() const;
    void setOutdoorTemp(int newOutdoorTemp);

    QString userName() const;
    void setUserName(const QString &newUserName);

    int batteryPercent() const;
    void setBatteryPercent(int newBatteryPercent);

    QString tempUnit() const;
    void setTempUnit(const QString &newTempUnit);

    QString speedUnit() const;
    void setSpeedUnit(const QString &newSpeedUnit);

    QColor interiorColor() const;
    void setInteriorColor(const QColor &newColor);

    int speedWarning() const;
    void setSpeedWarning(int newWarning);

    Q_INVOKABLE int convertCelsiusToFahrenheit(int temperature) const;
    Q_INVOKABLE int convertKilometersToMiles(int kilometers) const;
    Q_INVOKABLE int convertKMHToMPH(int kmh) const;

    QString distanceUnit() const;
    void setDistanceUnit(const QString &newDistanceUnit);

signals:

    void carLockedChanged(bool carLocked);
    void outdoorTempChanged(int outdoorTemp);
    void userNameChanged(QString userName);
    void batteryPercentChanged(int batteryPercent);
    void tempUnitChanged(QString tempUnit);
    void speedUnitChanged(QString speedUnit);
    void interiorColorChanged(QColor interiorColor);
    void speedWarningChanged(int speedWarning);

    void distanceUnitChanged();

private:
    bool m_carLocked;
    int m_outdoorTemp;
    QString m_userName;
    int m_batteryPercent;
    QString m_tempUnit = "C°";
    QString m_speedUnit = "km/h";
    QColor m_interiorColor = QColor(0xffffff);
    int m_speedWarning = 120;
    SettingsManager m_settings;
    QString m_distanceUnit;
};

#endif // SYSTEM_H
