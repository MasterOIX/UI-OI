#ifndef AUDIOCONTROLLER_H
#define AUDIOCONTROLLER_H

#include <QObject>

class AudioController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int volume READ volume WRITE setVolume NOTIFY volumeChanged)
public:
    explicit AudioController(QObject *parent = nullptr);

    int volume() const;
    void setVolume(int newVolume);

    Q_INVOKABLE void increaseVolume(const int &value);
    Q_INVOKABLE void playRadioStream(const QString &url);
    Q_INVOKABLE void stopRadioStream();

signals:
    void volumeChanged(int volume);

private:
    int m_volume;
};

#endif // AUDIOCONTROLLER_H
