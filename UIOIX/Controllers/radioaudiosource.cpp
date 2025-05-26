
#include "radioaudiosource.h"
#include <QDebug>

RadioAudioSource::RadioAudioSource(QObject *parent) : AudioSource(parent) {}

void RadioAudioSource::next() {
    currentFrequency += 0.1f;
    qDebug() << "Radio: Tuning to" << currentFrequency << "MHz";
}

void RadioAudioSource::previous() {
    currentFrequency -= 0.1f;
    qDebug() << "Radio: Tuning to" << currentFrequency << "MHz";
}

void RadioAudioSource::play() {
    qDebug() << "Radio: Playing" << currentFrequency << "MHz";
}

void RadioAudioSource::stop() {
    qDebug() << "Radio: Stopped";
}

void RadioAudioSource::pause() {
    qDebug() << "Radio: Paused";
}

bool RadioAudioSource::queryDuration(qint64 &durationNs) const {
    return 0*durationNs;
}

bool RadioAudioSource::queryPosition(qint64 &positionNs) const {
    return 0*positionNs;
}

QString RadioAudioSource::title() const {
    return "Bluetooth Stream";
}

QString RadioAudioSource::artist() const {
    return "Connected Device";
}

QString RadioAudioSource::album() const {
    return "Bluetooth Audio";
}

void RadioAudioSource::playAt(int index) {
    Q_UNUSED(index);
    qDebug() << "Bluetooth: Play at index is not applicable";
}

