
#ifndef AUDIOCONTROLLER_H
#define AUDIOCONTROLLER_H

#include <QObject>
#include <QTimer>
#include "audiosourcemanager.h"

class AudioController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int volume READ volume WRITE setVolume NOTIFY volumeChanged)
    Q_PROPERTY(PlaybackMode mode READ mode WRITE setMode NOTIFY modeChanged)
    Q_PROPERTY(QStringList localSongs READ localSongs NOTIFY localSongsChanged)
    Q_PROPERTY(bool isPlaying READ isPlaying WRITE setIsPlaying NOTIFY isPlayingChanged)
    Q_PROPERTY(double position READ position NOTIFY playbackInfoChanged)
    Q_PROPERTY(double duration READ duration NOTIFY playbackInfoChanged)
    Q_PROPERTY(QString currentTitle READ currentTitle NOTIFY metadataChanged)
    Q_PROPERTY(QString currentArtist READ currentArtist NOTIFY metadataChanged)
    Q_PROPERTY(QString currentAlbum READ currentAlbum NOTIFY metadataChanged)

public:
    explicit AudioController(QObject *parent = nullptr);
    ~AudioController();

    enum PlaybackMode {
        Radio,
        Bluetooth,
        Storage,
        Web
    };
    Q_ENUM(PlaybackMode)

    int volume() const;
    void setVolume(int newVolume);
    void setIsPlaying(bool isPlaying);

    Q_INVOKABLE void increaseVolume(const int &value);
    Q_INVOKABLE void toggleMode();
    Q_INVOKABLE void setMode(PlaybackMode newMode);
    Q_INVOKABLE PlaybackMode mode() const;

    Q_INVOKABLE void play();
    Q_INVOKABLE void stop();
    Q_INVOKABLE void next();
    Q_INVOKABLE void previous();
    Q_INVOKABLE void pause();

    Q_INVOKABLE void scanLocalMusic(const QString &path);
    Q_INVOKABLE void setSystemVolume(int volumePercent);
    Q_INVOKABLE int getSystemVolume() const;
    Q_INVOKABLE void togglePlayPause();
    Q_INVOKABLE void printPlaybackStatus();

    QStringList localSongs() const;

    bool isPlaying() const;
    double position() const;
    double duration() const;

    QString currentTitle() const;
    QString currentArtist() const;
    QString currentAlbum() const;

signals:
    void volumeChanged(int volume);
    void modeChanged();
    void localSongsChanged();

    void isPlayingChanged();
    void playbackInfoChanged();
    void metadataChanged();

private:
    int m_volume;
    QStringList m_localSongs;
    AudioSourceManager *m_sourceManager;
    bool m_isPlaying;
    QTimer *m_positionTimer;
};

#endif // AUDIOCONTROLLER_H
