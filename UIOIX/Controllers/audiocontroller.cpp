#include "audiocontroller.h"
#include "audiosourcemanager.h"
#include <alsa/asoundlib.h>
#include <QProcess>
#include <QSettings>
#include <QDir>
#include <QDebug>
#include <algorithm>

QProcess *mpvProcess = nullptr;

AudioController::AudioController(QObject *parent)
    : QObject(parent),
    m_volume(0),
    m_sourceManager(new AudioSourceManager(this)),
    m_isPlaying(true)
{
    int sysVol = getSystemVolume();
    setVolume(sysVol >= 0 ? sysVol : 50);

    QSettings settings("UIOIX", "UIOIApp");
    QString savedMode = settings.value("audio/lastMode", "Radio").toString();

    PlaybackMode restoredMode = Radio;
    if (savedMode == "Bluetooth") restoredMode = Bluetooth;
    else if (savedMode == "Storage") restoredMode = Storage;
    else if (savedMode == "Web") restoredMode = Web;

    setMode(restoredMode);

    m_positionTimer = new QTimer(this);
    connect(m_positionTimer, &QTimer::timeout, this, [this]() {
        emit playbackInfoChanged();
        printPlaybackStatus();
    });
    m_positionTimer->start(1000);
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
    emit playbackInfoChanged();
    emit metadataChanged();
    setIsPlaying(true);

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
    return m_localSongs;
}

void AudioController::scanLocalMusic(const QString &path) {
    QDir dir(path);
    if (!dir.exists()) return;

    QStringList filters { "*.mp3", "*.wav", "*.flac", "*.ogg", "*.aac" };
    QStringList songs = dir.entryList(filters, QDir::Files, QDir::Name);

    for (QString &song : songs)
        song = path + "/" + song;

    m_localSongs = songs;
    emit localSongsChanged();
}

void AudioController::next() {
    if (m_sourceManager->currentSource()) {
        m_sourceManager->currentSource()->next();
        setIsPlaying(true);
        emit playbackInfoChanged();
        emit metadataChanged();
    }
}

void AudioController::previous() {
    if (m_sourceManager->currentSource()) {
        m_sourceManager->currentSource()->previous();
        setIsPlaying(true);
        emit playbackInfoChanged();
        emit metadataChanged();
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
            return pos / 1e9;
    }
    return 0;
}

double AudioController::duration() const {
    if (m_sourceManager && m_sourceManager->currentSource()) {
        qint64 dur = 0;
        if (m_sourceManager->currentSource()->queryDuration(dur))
            return dur / 1e9;
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
        double posSec = pos / 1e9;
        double durSec = dur / 1e9;
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

