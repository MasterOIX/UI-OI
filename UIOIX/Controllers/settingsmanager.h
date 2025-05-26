#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QObject>
#include <QSettings>

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

private:
    QSettings settings;
};

#endif // SETTINGSMANAGER_H
