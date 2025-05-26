
#ifndef ONLINERADIOAUDIOSOURCE_H
#define ONLINERADIOAUDIOSOURCE_H

#include "audiosource.h"
#include <QStringList>
#include <gst/gst.h>

struct RadioStation {
    QString name;
    QString url;
};

class OnlineRadioAudioSource : public AudioSource {
public:
    explicit OnlineRadioAudioSource(QObject *parent = nullptr);
    ~OnlineRadioAudioSource() override;
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
    QList<RadioStation> stations;
    int currentIndex = 0;
    GstElement *m_player = nullptr;
};

#endif // ONLINERADIOAUDIOSOURCE_H
