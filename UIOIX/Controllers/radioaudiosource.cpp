#include "radioaudiosource.h"
#include <QDebug>
#include <QThread>
#include <QStringList>
#include <QUrl>

RadioAudioSource::RadioAudioSource(QObject *parent)
    : AudioSource(parent), currentIndex(0), m_player(nullptr) {

    stations = {
        { "Kiss FM",                    96.1 },
        { "Radio ZU",                   89.0 },
        { "Radio România Actualităţi", 91.8 },
        { "Europa FM",                106.7 },
        { "Digi FM",                  106.2 },
        { "Magic FM",                  90.8 },
        { "Pro FM",                   102.8 },
        { "Virgin Radio",             100.2 }
    };

    m_radio.reset();
    m_radio.initialize();
    m_radio.tuneFrequency(stations[currentIndex].frequency);
}

void RadioAudioSource::play() {
    m_radio.mute(false);
    m_radio.tuneFrequency(stations[currentIndex].frequency);

    if (m_player) {
        gst_element_set_state(m_player, GST_STATE_NULL);
        gst_object_unref(m_player);
        m_player = nullptr;
    }

    const char* pipelineDesc = "alsasrc ! audioconvert ! audioresample ! volume name=myvolume ! alsasink";
    m_player = gst_parse_launch(pipelineDesc, nullptr);

    if (!m_player) {
        qWarning() << "[RadioAudioSource] Failed to create GStreamer pipeline";
        return;
    }

    GstStateChangeReturn ret = gst_element_set_state(m_player, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE) {
        qWarning() << "[RadioAudioSource] Failed to start GStreamer pipeline";
    } else {
        qDebug() << "[RadioAudioSource] FM pipeline started for" << title();
    }
}

void RadioAudioSource::pause() {
    m_radio.mute(true);
    if (m_player) {
        gst_element_set_state(m_player, GST_STATE_PAUSED);
        qDebug() << "Radio: Paused";
    }
}

void RadioAudioSource::stop() {
    m_radio.mute(true);
    if (m_player) {
        gst_element_set_state(m_player, GST_STATE_NULL);
        gst_object_unref(m_player);
        m_player = nullptr;
        qDebug() << "Radio: Stopped";
    }
}

void RadioAudioSource::next() {
    currentIndex = (currentIndex + 1) % stations.size();
    play();
}

void RadioAudioSource::previous() {
    currentIndex = (currentIndex - 1 + stations.size()) % stations.size();
    play();
}

bool RadioAudioSource::queryDuration(qint64 &durationNs) const {
    Q_UNUSED(durationNs);
    return false;
}

bool RadioAudioSource::queryPosition(qint64 &positionNs) const {
    Q_UNUSED(positionNs);
    return false;
}

QString RadioAudioSource::title() const {
    return stations.value(currentIndex).name;
}

QString RadioAudioSource::artist() const {
    return QString::number(stations.value(currentIndex).frequency, 'f', 1) + " MHz";
}

QString RadioAudioSource::album() const {
    return "FM Radio";
}

QStringList RadioAudioSource::list() const {
    QStringList names;
    for (const auto &s : stations)
        names << s.name;
    return names;
}

void RadioAudioSource::playAt(int index) {
    if (index >= 0 && index < stations.size()) {
        currentIndex = index;
        play();
    }
}

void RadioAudioSource::setVolume(int volumePercent) {
    if (!m_player) {
        qWarning() << "[RadioAudioSource] GStreamer player is not initialized!";
        return;
    }

    double volume = qBound(0.0, volumePercent / 100.0, 1.0);
    GstElement *volumeElement = gst_bin_get_by_name(GST_BIN(m_player), "myvolume");
    if (!volumeElement) {
        qWarning() << "[RadioAudioSource] Volume element not found!";
        return;
    }

    g_object_set(G_OBJECT(volumeElement), "volume", volume, nullptr);
    gst_object_unref(volumeElement);  // important pentru a evita leak-uri

    qDebug() << "[RadioAudioSource] Set volume to:" << volume;
}
