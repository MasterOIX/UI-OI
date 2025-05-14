#include "audiocontroller.h"
#include "qdebug.h"
#include <algorithm>
#include <QProcess>

QProcess *mpvProcess = nullptr;

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

void AudioController::playRadioStream(const QString &url)
{
    stopRadioStream();  // stop any previous instance

    mpvProcess = new QProcess(this);
    mpvProcess->start("mpv", QStringList() << url);

    connect(mpvProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, []() { qDebug() << "Radio stream stopped."; });
}

void AudioController::stopRadioStream()
{
    if (mpvProcess && mpvProcess->state() != QProcess::NotRunning) {
        mpvProcess->kill();
        mpvProcess->waitForFinished();
        mpvProcess->deleteLater();
        mpvProcess = nullptr;
    }
}
