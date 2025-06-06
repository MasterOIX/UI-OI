#include "audiosourcemanager.h"

AudioSourceManager::AudioSourceManager(BluetoothController* btController, QObject *parent)
    : QObject(parent)
{
    m_current = nullptr;  // Do not set or play anything at startup
    connect(btController, &BluetoothController::bluetoothMediaPlayerPathChanged,
            &m_bt, &BluetoothAudioSource::setBluezDevicePath);
}

void AudioSourceManager::nextSource() {
    int next = static_cast<int>(m_mode) + 1;
    if (next > static_cast<int>(Web))
        next = static_cast<int>(Radio);

    setMode(static_cast<PlaybackMode>(next));
}

void AudioSourceManager::setMode(PlaybackMode mode) {
    if (m_current){
        m_current->setPlaying(false);
        m_current->stop();
    }

    m_mode = mode;
    updateSourcePointer();

    if (m_current) {
        disconnect(m_current, nullptr, this, nullptr);
        // connect signals to propagate up
        connect(m_current, &AudioSource::metadataChanged, this, [this]() {
            emit metadataChanged();
        });
        connect(m_current, &AudioSource::playbackInfoChanged, this, [this]() {
            emit playbackInfoChanged();
        });
        m_current->setPlaying(true);
        m_current->play();
    }
}

AudioSourceManager::PlaybackMode AudioSourceManager::currentMode() const {
    return m_mode;
}

AudioSource* AudioSourceManager::currentSource() const {
    return m_current;
}

void AudioSourceManager::updateSourcePointer() {
    switch (m_mode) {
    case Storage:   m_current = &m_storage; break;
    case Radio:     m_current = &m_radio; break;
    case Web:       m_current = &m_web; break;
    case Bluetooth: m_current = &m_bt; break;
    }
}
