#ifndef RADIOAUDIOSOURCE_H
#define RADIOAUDIOSOURCE_H

#include "Controllers/si4703controller.h"
#include "audiosource.h"
#include <QObject>
#include <QVector>
#include <gst/gst.h>

struct Station {
    QString name;
    double frequency;
};

class RadioAudioSource : public AudioSource
{
    Q_OBJECT

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
    void playAt(int index) override;
    void setVolume(int volumePercent) override;
    QStringList list() const override;

private:
    QList<Station> stations;
    Si4703Controller m_radio;
    int currentIndex = 0;
    GstElement* m_player = nullptr;
};

#endif // RADIOAUDIOSOURCE_H
