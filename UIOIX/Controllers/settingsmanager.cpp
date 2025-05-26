#include "settingsmanager.h"

SettingsManager::SettingsManager(QObject *parent)
    : QObject(parent),
    settings("UIOIX", "UIOIApp") // Change as needed
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
