#include "settingsmanager.h"
#include "qcolor.h"

SettingsManager::SettingsManager(QObject *parent)
    : QObject(parent),
    settings("UIOIX", "UIOIApp")
{
}

void SettingsManager::saveAudioMode(const QString &mode) {
    settings.setValue("audio/lastMode", mode);
}

QString SettingsManager::loadAudioMode() {
    return settings.value("audio/lastMode", "FM").toString();
}

void SettingsManager::saveKilometers(int km) {
    settings.setValue("vehicle/km", km);
}

int SettingsManager::loadKilometers() {
    return settings.value("vehicle/km", 0).toInt();
}

void SettingsManager::saveHVACTemp(double temp) {
    settings.setValue("hvac/lastTemp", temp);
}

double SettingsManager::loadHVACTemp() {
    return settings.value("hvac/lastTemp", 22.0).toDouble();
}

void SettingsManager::saveInteriorColor(const QColor &color) {
    settings.setValue("display/interiorColor", color.name(QColor::HexArgb));
}

QColor SettingsManager::loadInteriorColor() {
    return QColor(settings.value("display/interiorColor", "#FFFFFFFF").toString());
}

void SettingsManager::saveTempUnit(const QString &unit) {
    settings.setValue("units/temp", unit);
}

QString SettingsManager::loadTempUnit() {
    return settings.value("units/temp", "°C").toString();
}

void SettingsManager::saveSpeedUnit(const QString &unit) {
    settings.setValue("units/speed", unit);
}

QString SettingsManager::loadSpeedUnit() {
    return settings.value("units/speed", "km/h").toString();
}

void SettingsManager::saveSpeedWarning(int speed) {
    settings.setValue("warnings/speed", speed);
}

int SettingsManager::loadSpeedWarning() {
    return settings.value("warnings/speed", 120).toInt();
}

void SettingsManager::saveTotalDistance(int distance) {
    settings.setValue("vehicle/totalDistance", distance);
}

int SettingsManager::loadTotalDistance() {
    return settings.value("vehicle/totalDistance", 0).toInt();
}

void SettingsManager::saveRange(int range) {
    settings.setValue("vehicle/range", range);
}

int SettingsManager::loadRange() {
    return settings.value("vehicle/range", 0).toInt();
}

void SettingsManager::saveConsumption(double consumption) {
    settings.setValue("vehicle/consumption", consumption);
}

double SettingsManager::loadConsumption() {
    return settings.value("vehicle/consumption", 0.0).toDouble();
}

void SettingsManager::saveAverageSpeed(int speed) {
    settings.setValue("vehicle/averageSpeed", speed);
}

int SettingsManager::loadAverageSpeed() {
    return settings.value("vehicle/averageSpeed", 0).toInt();
}

void SettingsManager::saveStartTime(const QString &time) {
    settings.setValue("vehicle/startTime", time);
}

QString SettingsManager::loadStartTime() {
    return settings.value("vehicle/startTime", "00:00").toString();
}

void SettingsManager::saveTripDistance(int distance) {
    settings.setValue("trip/distance", distance);
}

int SettingsManager::loadTripDistance() {
    return settings.value("trip/distance", 0).toInt();
}

void SettingsManager::saveTripDuration(const QString &duration) {
    settings.setValue("trip/duration", duration);
}

QString SettingsManager::loadTripDuration() {
    return settings.value("trip/duration", "0:00hr").toString();
}

void SettingsManager::saveTripConsumption(double consumption) {
    settings.setValue("trip/consumption", consumption);
}

double SettingsManager::loadTripConsumption() {
    return settings.value("trip/consumption", 0.0).toDouble();
}

void SettingsManager::saveTripAverageSpeed(int speed) {
    settings.setValue("trip/averageSpeed", speed);
}

int SettingsManager::loadTripAverageSpeed() {
    return settings.value("trip/averageSpeed", 0).toInt();
}

void SettingsManager::saveResetAutomatically(bool value) {
    settings.setValue("trip/resetAutomatically", value);
}

bool SettingsManager::loadResetAutomatically() {
    return settings.value("trip/resetAutomatically", false).toBool();
}
