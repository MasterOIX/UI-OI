
#include "bluetoothaudiosource.h"
#include <QDebug>

BluetoothAudioSource::BluetoothAudioSource(QObject *parent) : AudioSource(parent) {}

void BluetoothAudioSource::next() {
    qDebug() << "Bluetooth: Sending AVRCP NEXT command";
}

void BluetoothAudioSource::previous() {
    qDebug() << "Bluetooth: Sending AVRCP PREVIOUS command";
}

void BluetoothAudioSource::play() {
    qDebug() << "Bluetooth: Play command issued";
}

void BluetoothAudioSource::stop() {
    qDebug() << "Bluetooth: Stop command issued";
}

void BluetoothAudioSource::pause() {
    qDebug() << "Bluetooth: Pause command issued";
}

bool BluetoothAudioSource::queryDuration(qint64 &durationNs) const {
    return 0*durationNs;
}

bool BluetoothAudioSource::queryPosition(qint64 &positionNs) const {
    return 0*positionNs;
}

QString BluetoothAudioSource::title() const {
    return "Bluetooth Stream";
}

QString BluetoothAudioSource::artist() const {
    return "Connected Device";
}

QString BluetoothAudioSource::album() const {
    return "Bluetooth Audio";
}

