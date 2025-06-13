#include "onlineradioaudiosource.h"
#include <QDebug>
#include <QUrl>

OnlineRadioAudioSource::OnlineRadioAudioSource(QObject *parent)
    : AudioSource(parent), currentIndex(0), m_player(nullptr) {
    stations = {
        { "Radio Guerrilla", "https://live.guerrillaradio.ro:8443/guerrilla.aac" },
        { "Kiss FM",         "https://live.kissfm.ro/kissfm.aacp" },
        { "Europa FM",       "https://astreaming.edi.ro:8443/EuropaFM_aac" },
        { "PRO FM",          "https://edge126.rcs-rds.ro/profm/profm.mp3" },
        { "Magic FM",        "https://live.magicfm.ro/magicfm.aacp" },
        { "Radio ZU",        "https://ssl.servereradio.ro/8054/stream" },
        { "Pescobar",        "https://play.radioking.io/pescobar" }
    };
}

OnlineRadioAudioSource::~OnlineRadioAudioSource() {
    if (m_player) {
        gst_element_set_state(m_player, GST_STATE_NULL);
        gst_object_unref(m_player);
        m_player = nullptr;
    }
}

void OnlineRadioAudioSource::play() {
    if (stations.isEmpty()) return;

    QString uri = stations[currentIndex].url;
    qDebug() << "[OnlineRadio] Playing" << stations[currentIndex].name << uri;

    // Cleanup previous player
    if (m_player) {
        gst_element_set_state(m_player, GST_STATE_NULL);
        gst_object_unref(m_player);
        m_player = nullptr;
    }

    m_player = gst_parse_launch(QString("playbin uri=\"%1\"").arg(uri).toUtf8().constData(), nullptr);
    if (!m_player) {
        qWarning() << "[OnlineRadio] Failed to create playbin for URI:" << uri;
        return;
    }

    GstStateChangeReturn ret = gst_element_set_state(m_player, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE) {
        qWarning() << "[OnlineRadio] Failed to play URI:" << uri;
    }
}

void OnlineRadioAudioSource::pause() {
    if (m_player) {
        gst_element_set_state(m_player, GST_STATE_PAUSED);
        qDebug() << "OnlineRadio: Paused";
    }
}

void OnlineRadioAudioSource::stop() {
    if (m_player) {
        gst_element_set_state(m_player, GST_STATE_NULL);
        qDebug() << "OnlineRadio: Stopped";
    }
}

void OnlineRadioAudioSource::next() {
    if (currentIndex < stations.size() - 1) {
        currentIndex++;
    } else {
        currentIndex = 0; // loop
    }
    play();
}

void OnlineRadioAudioSource::previous() {
    if (currentIndex > 0) {
        currentIndex--;
    } else {
        currentIndex = stations.size() - 1; // loop
    }
    play();
}

bool OnlineRadioAudioSource::queryDuration(qint64 &durationNs) const {
    return 0*durationNs;
}

bool OnlineRadioAudioSource::queryPosition(qint64 &positionNs) const {
    return 0*positionNs;
}

QString OnlineRadioAudioSource::title() const {
    return stations.value(currentIndex).name;
}

QString OnlineRadioAudioSource::artist() const {
    return stations.value(currentIndex).name;
}

QString OnlineRadioAudioSource::album() const {
    return stations.value(currentIndex).name;
}

QStringList OnlineRadioAudioSource::list() const {
    QStringList names;
    for (const auto& station : stations) {
        names << station.name;
    }
    return names;
}

void OnlineRadioAudioSource::playAt(int index) {
    if (index >= 0 && index < stations.size()) {
        currentIndex = index;
        play();
    }
}

void OnlineRadioAudioSource::setVolume(int volumePercent) {
    if (!m_player) {
        qWarning() << "[OnlineRadioAudioSource] GStreamer player is not initialized!";
        return;
    }

    double volume = qBound(0.0, volumePercent / 100.0, 1.0);
    g_object_set(G_OBJECT(m_player), "volume", volume, nullptr);
    qDebug() << "[OnlineRadioAudioSource] Set playbin volume to:" << volume;
}

