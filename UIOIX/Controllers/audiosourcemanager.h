
#ifndef AUDIOSOURCEMANAGER_H
#define AUDIOSOURCEMANAGER_H

#include "audiosource.h"
#include "storageaudiosource.h"
#include "radioaudiosource.h"
#include "onlineradioaudiosource.h"
#include "bluetoothaudiosource.h"

class AudioSourceManager : public QObject {
    Q_OBJECT
public:
    explicit AudioSourceManager(QObject *parent = nullptr);

    enum PlaybackMode {
        Radio,
        Bluetooth,
        Storage,
        Web
    };
    Q_ENUM(PlaybackMode)

    void nextSource();                     // switch to next mode
    void setMode(PlaybackMode mode);      // explicitly set mode
    PlaybackMode currentMode() const;

    AudioSource *currentSource() const;   // get currently active source

private:
    PlaybackMode m_mode;
    AudioSource *m_current = nullptr;

    StorageAudioSource m_storage;
    RadioAudioSource m_radio;
    OnlineRadioAudioSource m_web;
    BluetoothAudioSource m_bt;

    void updateSourcePointer();
};

#endif // AUDIOSOURCEMANAGER_H
