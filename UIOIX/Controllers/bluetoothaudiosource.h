
#ifndef BLUETOOTHAUDIOSOURCE_H
#define BLUETOOTHAUDIOSOURCE_H

#include "audiosource.h"

class BluetoothAudioSource : public AudioSource {
public:
    explicit BluetoothAudioSource(QObject *parent = nullptr);
    void next() override;
    void previous() override;
    void play() override;
    void stop() override;
    void pause() override;

    bool queryDuration(qint64 &durationNs) const override;
    bool queryPosition(qint64 &positionNs) const override;

    QString title() const override;
    QString artist() const override;
    QString album() const override;
};

#endif // BLUETOOTHAUDIOSOURCE_H
