#include "audiosourcemanager.h"

AudioSourceManager::AudioSourceManager(QObject *parent)
    : QObject(parent)
{
    m_current = nullptr;  // Do not set or play anything at startup
}

void AudioSourceManager::nextSource() {
    int next = static_cast<int>(m_mode) + 1;
    if (next > static_cast<int>(Web))
        next = static_cast<int>(Radio);

    setMode(static_cast<PlaybackMode>(next));
}

void AudioSourceManager::setMode(PlaybackMode mode) {
    if (m_current)
        m_current->stop();

    m_mode = mode;
    updateSourcePointer();

    if (m_current)
        m_current->play();
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

