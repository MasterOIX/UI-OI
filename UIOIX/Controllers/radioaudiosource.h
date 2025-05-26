
#ifndef RADIOAUDIOSOURCE_H
#define RADIOAUDIOSOURCE_H

#include "audiosource.h"

class RadioAudioSource : public AudioSource {
public:
    explicit RadioAudioSource(QObject *parent = nullptr);
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

private:
    float currentFrequency = 100.1f; // MHz
};

#endif // RADIOAUDIOSOURCE_H
