#include "audiocontroller.h"
#include "Controllers/bluetoothcontroller.h"
#include <alsa/asoundlib.h>
#include <QProcess>
#include <QSettings>
#include <QDir>
#include <QDebug>
#include <algorithm>

AudioController::AudioController(BluetoothController* btController, QObject* parent)
    : QObject(parent),
    m_volume(0),
    m_isPlaying(true),
    m_bt(this)
{
    m_positionTimer = new QTimer(this);
    connect(m_positionTimer, &QTimer::timeout, this, [this]() {
        emit playbackInfoChanged();
        printPlaybackStatus();
    });
    m_sourceManager = new AudioSourceManager(btController, this);

    int sysVol = getSystemVolume();
    setVolume(sysVol >= 0 ? sysVol : 50);

    QSettings settings("UIOIX", "UIOIApp");
    QString savedMode = settings.value("audio/lastMode", "Radio").toString();

    PlaybackMode restoredMode = Radio;
    if (savedMode == "Bluetooth") restoredMode = Bluetooth;
    else if (savedMode == "Storage") restoredMode = Storage;
    else if (savedMode == "Web") restoredMode = Web;

    setMode(restoredMode);

    // Connect metadata and playback info signals from AudioSourceManager
    connect(m_sourceManager, &AudioSourceManager::metadataChanged, this, &AudioController::metadataChanged);
    connect(m_sourceManager, &AudioSourceManager::playbackInfoChanged, this, &AudioController::playbackInfoChanged);
}

AudioController::~AudioController() {
    delete m_sourceManager;
}

void AudioController::setSystemVolume(int volumePercent) {
    volumePercent = std::clamp(volumePercent, 0, 100);

    snd_mixer_t *handle = nullptr;
    snd_mixer_selem_id_t *sid = nullptr;

    const char *card = "default";
    const char *selem_name = "Master";

    if (snd_mixer_open(&handle, 0) < 0) return;
    if (snd_mixer_attach(handle, card) < 0 ||
        snd_mixer_selem_register(handle, nullptr, nullptr) < 0 ||
        snd_mixer_load(handle) < 0) {
        snd_mixer_close(handle);
        return;
    }

    snd_mixer_selem_id_malloc(&sid);
    snd_mixer_selem_id_set_index(sid, 0);
    snd_mixer_selem_id_set_name(sid, selem_name);

    snd_mixer_elem_t *elem = snd_mixer_find_selem(handle, sid);
    if (!elem) {
        snd_mixer_selem_id_free(sid);
        snd_mixer_close(handle);
        return;
    }

    long min, max;
    snd_mixer_selem_get_playback_volume_range(elem, &min, &max);
    long volume = min + ((max - min) * volumePercent) / 100;
    snd_mixer_selem_set_playback_volume_all(elem, volume);
    snd_mixer_selem_set_playback_switch_all(elem, 1);

    snd_mixer_close(handle);
    snd_mixer_selem_id_free(sid);
}

int AudioController::getSystemVolume() const {
    snd_mixer_t *handle;
    snd_mixer_selem_id_t *sid;
    const char *card = "default";
    const char *selem_name = "Master";

    long min, max, vol;

    if (snd_mixer_open(&handle, 0) < 0) return -1;
    snd_mixer_attach(handle, card);
    snd_mixer_selem_register(handle, nullptr, nullptr);
    snd_mixer_load(handle);

    snd_mixer_selem_id_malloc(&sid);
    snd_mixer_selem_id_set_index(sid, 0);
    snd_mixer_selem_id_set_name(sid, selem_name);

    snd_mixer_elem_t *elem = snd_mixer_find_selem(handle, sid);
    if (!elem) return -1;

    snd_mixer_selem_get_playback_volume_range(elem, &min, &max);
    snd_mixer_selem_get_playback_volume(elem, SND_MIXER_SCHN_FRONT_LEFT, &vol);

    snd_mixer_close(handle);
    snd_mixer_selem_id_free(sid);

    return static_cast<int>((vol - min) * 100 / (max - min));
}

void AudioController::togglePlayPause() {
    if (m_sourceManager->currentSource()) {
        if (m_isPlaying) {
            m_sourceManager->currentSource()->pause();
            setIsPlaying(false);
        } else {
            m_sourceManager->currentSource()->play();
            setIsPlaying(true);
        }
    }
}

int AudioController::volume() const {
    return m_volume;
}

void AudioController::setVolume(int newVolume) {
    newVolume = std::clamp(newVolume, 0, 100);
    if (m_volume == newVolume) return;
    m_volume = newVolume;
    setSystemVolume(newVolume);
    emit volumeChanged(m_volume);
}

void AudioController::increaseVolume(const int &value) {
    setVolume(m_volume + value);
}

void AudioController::toggleMode() {
    m_sourceManager->nextSource();
    setMode(static_cast<PlaybackMode>(m_sourceManager->currentMode()));
}

AudioController::PlaybackMode AudioController::mode() const {
    return static_cast<PlaybackMode>(m_sourceManager->currentMode());
}

void AudioController::setMode(PlaybackMode newMode) {
    if (static_cast<PlaybackMode>(m_sourceManager->currentMode()) != newMode) {
        m_sourceManager->setMode(static_cast<AudioSourceManager::PlaybackMode>(newMode));
    }

    emit modeChanged();
    emit metadataChanged();
    emit playbackInfoChanged();
    emit audioListChanged();
    setIsPlaying(true);

    if (newMode == Storage || newMode == Bluetooth)  {
        m_positionTimer->start(1000);
    } else {
        m_positionTimer->stop();
    }


    QSettings settings("UIOIX", "UIOIApp");
    QString modeStr;
    switch (newMode) {
    case Bluetooth: modeStr = "Bluetooth"; break;
    case Storage:   modeStr = "Storage";   break;
    case Web:       modeStr = "Web";       break;
    default:        modeStr = "Radio";     break;
    }

    settings.setValue("audio/lastMode", modeStr);
    qDebug() << "Audio mode saved as:" << modeStr;
}

QStringList AudioController::localSongs() const {
    return m_sourceManager->storageSource()->songList();
}

void AudioController::next() {
    if (m_sourceManager->currentSource()) {
        m_sourceManager->currentSource()->next();
        setIsPlaying(true);
        emit metadataChanged();
        emit playbackInfoChanged();
    }
}

void AudioController::previous() {
    if (m_sourceManager->currentSource()) {
        m_sourceManager->currentSource()->previous();
        setIsPlaying(true);
        emit metadataChanged();
        emit playbackInfoChanged();
    }
}

void AudioController::pause() {
    if (m_sourceManager->currentSource()) {
        m_sourceManager->currentSource()->pause();
        setIsPlaying(false);
    }
}

void AudioController::play() {
    if (m_sourceManager->currentSource()) {
        m_sourceManager->currentSource()->play();
        setIsPlaying(true);
    }
}

void AudioController::stop() {
    if (m_sourceManager->currentSource())
        m_sourceManager->currentSource()->stop();
}

bool AudioController::isPlaying() const {
    return m_isPlaying;
}

void AudioController::setIsPlaying(bool isPlaying) {
    if (m_isPlaying == isPlaying)
        return;

    m_isPlaying = isPlaying;
    emit isPlayingChanged();
}

double AudioController::position() const {
    if (m_sourceManager && m_sourceManager->currentSource()) {
        qint64 pos = 0;
        if (m_sourceManager->currentSource()->queryPosition(pos))
            return pos;
    }
    return 0;
}

double AudioController::duration() const {
    if (m_sourceManager && m_sourceManager->currentSource()) {
        qint64 dur = 0;
        if (m_sourceManager->currentSource()->queryDuration(dur)){
            return dur;
        }
    }
    return 0;
}

void AudioController::printPlaybackStatus() {
    if (!m_sourceManager || !m_sourceManager->currentSource())
        return;

    qint64 pos = 0;
    qint64 dur = 0;

    if (m_sourceManager->currentSource()->queryPosition(pos) &&
        m_sourceManager->currentSource()->queryDuration(dur) &&
        dur > 0)
    {
        double posSec = pos;
        double durSec = dur;
        double percent = (posSec * 100.0) / durSec;

        qDebug().nospace()
            << "⏱ " << posSec << "s / " << durSec << "s  |  "
            << "📊 " << QString::number(percent, 'f', 1) << "% played";
    }
}

QString AudioController::currentTitle() const {
    return m_sourceManager->currentSource() ? m_sourceManager->currentSource()->title() : "No title";
}
QString AudioController::currentArtist() const {
    return m_sourceManager->currentSource() ? m_sourceManager->currentSource()->artist() : "No artist";
}
QString AudioController::currentAlbum() const {
    return m_sourceManager->currentSource() ? m_sourceManager->currentSource()->album() : "No album";
}

QStringList AudioController::audioList() const
{
    if (m_sourceManager && m_sourceManager->currentSource())
        return m_sourceManager->currentSource()->list();
    return {};
}

void AudioController::selectFromList(int index) {
    if (m_sourceManager && m_sourceManager->currentSource()) {
        m_sourceManager->currentSource()->playAt(index);
        setIsPlaying(true);
        emit playbackInfoChanged();
        emit metadataChanged();
    }
}

QString AudioController::usbPath() const
{
    if (m_sourceManager)
        return m_sourceManager->storageSource()->detectUsbPath();
    return QString();
}

void AudioController::importFromUsb()
{
    if (m_sourceManager) {
        bool result = m_sourceManager->storageSource()->importFromUsb();
        if (result) {
            emit localSongsChanged();
        } else {
            qWarning() << "Failed to import from USB";
        }
    }
}

int AudioController::sourceVolume() const
{
    return m_sourceVolume;
}

void AudioController::setSourceVolume(int newSourceVolume)
{
    if (m_sourceVolume == newSourceVolume)
        return;
    m_sourceVolume = newSourceVolume;
    emit sourceVolumeChanged();
}

void AudioController::scanLocalMusic(const QString &path)
{
    if (m_sourceManager) {
        m_sourceManager->storageSource()->scanLocalMusic(path);
        emit localSongsChanged();
        if (m_sourceManager->currentSource() == m_sourceManager->storageSource()) {
            emit audioListChanged();
        }
    }
}

void AudioController::playFromLocal(int index)
{
    if (m_sourceManager) {
        if (m_sourceManager->currentSource() != m_sourceManager->storageSource())
            setMode(Storage);
        m_sourceManager->storageSource()->playAt(index);
    }
}

double AudioController::percent() const
{
    double dur = duration();
    double pos = position();

    if (dur > 0.0)
        return pos / dur;

    return 0.0;
}
