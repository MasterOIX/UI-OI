
#ifndef AUDIOSOURCE_H
#define AUDIOSOURCE_H

#include <QObject>

class AudioSource : public QObject {
    Q_OBJECT
public:
    explicit AudioSource(QObject *parent = nullptr) : QObject(parent) {}
    virtual void next() = 0;
    virtual void previous() = 0;
    virtual void play() = 0;
    virtual void stop() = 0;
    virtual void pause() = 0;

    virtual bool queryDuration(qint64 &durationNs) const = 0;
    virtual bool queryPosition(qint64 &positionNs) const = 0;

    virtual QString title() const = 0;
    virtual QString artist() const = 0;
    virtual QString album() const = 0;
    virtual QStringList list() const = 0;
    virtual void playAt(int index) = 0;
    virtual void setVolume(int volumePercent) = 0;
};

#endif // AUDIOSOURCE_H
