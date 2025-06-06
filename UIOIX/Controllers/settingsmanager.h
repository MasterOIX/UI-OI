#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QObject>
#include <QSettings>
#include <QColor>

class SettingsManager : public QObject
{
    Q_OBJECT

public:
    explicit SettingsManager(QObject *parent = nullptr);

    Q_INVOKABLE void saveAudioMode(const QString &mode);
    Q_INVOKABLE QString loadAudioMode();

    Q_INVOKABLE void saveKilometers(int km);
    Q_INVOKABLE int loadKilometers();

    Q_INVOKABLE void saveHVACTemp(double temp);
    Q_INVOKABLE double loadHVACTemp();

    Q_INVOKABLE void saveInteriorColor(const QColor &color);
    Q_INVOKABLE QColor loadInteriorColor();

    Q_INVOKABLE void saveTempUnit(const QString &unit);
    Q_INVOKABLE QString loadTempUnit();

    Q_INVOKABLE void saveSpeedUnit(const QString &unit);
    Q_INVOKABLE QString loadSpeedUnit();

    Q_INVOKABLE void saveSpeedWarning(int speed);
    Q_INVOKABLE int loadSpeedWarning();

    Q_INVOKABLE void saveTotalDistance(int distance);
    Q_INVOKABLE int loadTotalDistance();

    Q_INVOKABLE void saveRange(int range);
    Q_INVOKABLE int loadRange();

    Q_INVOKABLE void saveConsumption(double consumption);
    Q_INVOKABLE double loadConsumption();

    Q_INVOKABLE void saveAverageSpeed(int speed);
    Q_INVOKABLE int loadAverageSpeed();

    Q_INVOKABLE void saveStartTime(const QString &time);
    Q_INVOKABLE QString loadStartTime();

    Q_INVOKABLE void saveTripDistance(int distance);
    Q_INVOKABLE int loadTripDistance();

    Q_INVOKABLE void saveTripDuration(const QString &duration);
    Q_INVOKABLE QString loadTripDuration();

    Q_INVOKABLE void saveTripConsumption(double consumption);
    Q_INVOKABLE double loadTripConsumption();

    Q_INVOKABLE void saveTripAverageSpeed(int speed);
    Q_INVOKABLE int loadTripAverageSpeed();

    Q_INVOKABLE void saveResetAutomatically(bool reset);
    Q_INVOKABLE bool loadResetAutomatically();

private:
    QSettings settings;
};

#endif // SETTINGSMANAGER_H
