#include "audiocontroller.h"
#include <algorithm>

AudioController::AudioController(QObject *parent)
    : QObject(parent),
    m_volume(0)
{

}

int AudioController::volume() const
{
    return m_volume;
}

void AudioController::setVolume(int newVolume)
{
    if (m_volume == newVolume)
        return;
    m_volume = newVolume;
    emit volumeChanged(m_volume);
}

void AudioController::increaseVolume(const int &value)
{
    setVolume(std::clamp(m_volume + value, 0, 100));
}
