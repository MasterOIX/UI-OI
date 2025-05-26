
#ifndef STORAGEAUDIOSOURCE_H
#define STORAGEAUDIOSOURCE_H

#include "audiosource.h"
#include <QStringList>
#include <gst/gst.h>

class StorageAudioSource : public AudioSource {
    Q_OBJECT
public:
    explicit StorageAudioSource(QObject *parent = nullptr);
    void previous() override;
    void play() override;
    void stop() override;
    void pause() override;

    bool queryDuration(qint64 &durationNs) const override;
    bool queryPosition(qint64 &positionNs) const override;

    QString title() const override;
    QString artist() const override;
    QString album() const override;

    void scanLocalMusic(const QString &path);
    QStringList songList() const;

public slots:
    void next() override;

signals:
    void songListChanged();

private:
    QStringList m_songs;
    int currentIndex = 0;
    GstElement *m_player = nullptr;
    QString m_currentUri;
    QString m_title = "";
    QString m_artist = "";
    QString m_album = "";
};

#endif // STORAGEAUDIOSOURCE_H
