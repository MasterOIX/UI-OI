#include "storageaudiosource.h"
#include <QUrl>
#include <QDebug>
#include <QDir>
#include <gst/gst.h>

StorageAudioSource::StorageAudioSource(QObject *parent) : AudioSource(parent) {
    gst_init(nullptr, nullptr);
    scanLocalMusic("/home/root/music");
}

void StorageAudioSource::scanLocalMusic(const QString &path)
{
    QDir dir(path);
    if (!dir.exists()) return;

    QStringList filters{ "*.mp3", "*.wav", "*.flac", "*.ogg", "*.aac" };
    QStringList files = dir.entryList(filters, QDir::Files, QDir::Name);

    m_songs.clear();
    for (const auto &song : files) {
        m_songs << dir.absoluteFilePath(song);
    }

    currentIndex = 0;
    emit songListChanged();
}

QStringList StorageAudioSource::songList() const {
    return m_songs;
}

void StorageAudioSource::next() {
    if (m_songs.isEmpty()) return;

    if (currentIndex < m_songs.size() - 1) {
        ++currentIndex;
    } else {
        currentIndex = 0; // loop back
    }
    play();
}

void StorageAudioSource::previous() {
    if (m_songs.isEmpty()) return;

    if (currentIndex > 0) {
        --currentIndex;
    } else {
        currentIndex = m_songs.size() - 1;
    }
    play();
}

void StorageAudioSource::play()
{
    if (m_songs.isEmpty()) return;

    QString uri = QUrl::fromLocalFile(m_songs.at(currentIndex)).toString(QUrl::FullyEncoded);

    // Rebuild pipeline if changed
    if (!m_player || uri != m_currentUri) {
        if (m_player) {
            gst_element_set_state(m_player, GST_STATE_NULL);
            gst_object_unref(m_player);
            m_player = nullptr;
            m_title.clear();
            m_artist.clear();
            m_album.clear();
        }

        m_player = gst_parse_launch(QString("playbin uri=\"%1\"").arg(uri).toUtf8().constData(), nullptr);
        if (!m_player) {
            qWarning() << "[GStreamer] Failed to create playbin";
            return;
        }

        m_currentUri = uri;
        qDebug() << "[GStreamer] Created new pipeline for:" << uri;

        // Setup bus listener for EOS
        GstBus *bus = gst_element_get_bus(m_player);
        gst_bus_add_watch(bus, +[](GstBus *, GstMessage *msg, gpointer user_data) -> gboolean {
            auto *self = static_cast<StorageAudioSource *>(user_data);

            switch (GST_MESSAGE_TYPE(msg)) {
            case GST_MESSAGE_EOS:
                qDebug() << "[GStreamer] Reached end of stream, going to next track.";
                QMetaObject::invokeMethod(self, "next", Qt::QueuedConnection);
                break;

            case GST_MESSAGE_TAG: {
                GstTagList *tags = nullptr;
                gst_message_parse_tag(msg, &tags);
                if (tags) {
                    gchar *str = nullptr;

                    if (gst_tag_list_get_string(tags, GST_TAG_TITLE, &str)) {
                        self->m_title = QString::fromUtf8(str);
                        g_free(str);
                    }

                    if (gst_tag_list_get_string(tags, GST_TAG_ARTIST, &str)) {
                        self->m_artist = QString::fromUtf8(str);
                        g_free(str);
                    }

                    if (gst_tag_list_get_string(tags, GST_TAG_ALBUM, &str)) {
                        self->m_album = QString::fromUtf8(str);
                        g_free(str);
                    }

                    gst_tag_list_unref(tags);
                }
                break;
            }

            default:
                break;
            }

            return TRUE;
        }, this);

    } else {
        qDebug() << "[GStreamer] Resuming playback for:" << uri;
    }

    GstStateChangeReturn ret = gst_element_set_state(m_player, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE) {
        qWarning() << "[GStreamer] Failed to play";
    } else {
        qDebug() << "[GStreamer] Playback started or resumed";
    }
}

void StorageAudioSource::stop()
{
    if (m_player) {
        gst_element_set_state(m_player, GST_STATE_NULL);
        gst_object_unref(GST_OBJECT(m_player));
        m_player = nullptr;
        qDebug() << "[GStreamer] Stopped";
    }
}

void StorageAudioSource::pause()
{
    if (m_player) {
        GstStateChangeReturn ret = gst_element_set_state(m_player, GST_STATE_PAUSED);
        if (ret == GST_STATE_CHANGE_FAILURE) {
            qWarning() << "[GStreamer] Failed to pause";
        }
    }
}

bool StorageAudioSource::queryDuration(qint64 &durationNs) const {
    if (!m_player) return false;
    gint64 rawDuration = 0;
    bool success = gst_element_query_duration(m_player, GST_FORMAT_TIME, &rawDuration);
    durationNs = static_cast<qint64>(rawDuration);
    return success;
}

bool StorageAudioSource::queryPosition(qint64 &positionNs) const {
    if (!m_player) return false;
    gint64 rawPosition = 0;
    bool success = gst_element_query_position(m_player, GST_FORMAT_TIME, &rawPosition);
    positionNs = static_cast<qint64>(rawPosition);
    return success;
}

QString StorageAudioSource::title() const {
    return m_title.isEmpty() ? QFileInfo(m_songs.value(currentIndex)).baseName() : m_title;
}

QString StorageAudioSource::artist() const {
    return m_artist.isEmpty() ? "No artist" : m_artist;
}

QString StorageAudioSource::album() const {
    return m_album.isEmpty() ? "No album" : m_album;
}
