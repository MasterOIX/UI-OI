#include "storageaudiosource.h"
#include "qdiriterator.h"
#include "qstandardpaths.h"
#include <QUrl>
#include <QDebug>
#include <QDir>
#include <gst/gst.h>

StorageAudioSource::StorageAudioSource(QObject *parent) : AudioSource(parent) {
    gst_init(nullptr, nullptr);
    scanLocalMusic(QStandardPaths::writableLocation(QStandardPaths::MusicLocation));

}

void StorageAudioSource::scanLocalMusic(const QString &path)
{
    QDir dir(path);
    qDebug() << "[StorageAudioSource] Scanning local music in:" << path;
    if (!dir.exists()) return;

    m_songs.clear();

    // Save current path so you can navigate back
    m_currentPath = dir.absolutePath();


    // Add "⏎ ../" entry if we're not at the root
    if (QStandardPaths::writableLocation(QStandardPaths::MusicLocation) != path) {
        m_songs << ".."; // This will show as ⏎ ../ in QML
    }

    // Add folders that contain audio files
    QStringList subDirs = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (const QString &subDir : subDirs) {
        QDir subPath(dir.filePath(subDir));
        QStringList musicFiles = subPath.entryList({ "*.mp3", "*.wav", "*.flac", "*.ogg", "*.aac" }, QDir::Files);
        if (!musicFiles.isEmpty()) {
            m_songs << subPath.absolutePath();
        }
    }

    // Add audio files in the current folder
    QStringList filters{ "*.mp3", "*.wav", "*.flac", "*.ogg", "*.aac" };
    QStringList files = dir.entryList(filters, QDir::Files, QDir::Name);
    for (const QString &song : std::as_const(files)) {
        m_songs << dir.absoluteFilePath(song);
    }

    currentIndex = 0;
    emit songListChanged();
}



QStringList StorageAudioSource::songList() const {
    return m_songs;
}

void StorageAudioSource::setVolume(int volumePercent) {
    if (!m_player) {
        qWarning() << "[OnlineRadioAudioSource] GStreamer player is not initialized!";
        return;
    }

    double volume = qBound(0.0, volumePercent / 100.0, 1.0);
    g_object_set(G_OBJECT(m_player), "volume", volume, nullptr);
    qDebug() << "[OnlineRadioAudioSource] Set playbin volume to:" << volume;
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
    if (m_songs.isEmpty() || currentIndex >= m_songs.size()) return;

    QFileInfo info(m_songs.at(currentIndex));
    if (info.isDir()) {
        currentIndex++;
        play(); // Recursively play next item
        return;
    }
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
        emit metadataChanged();
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
    durationNs = static_cast<qint64>(rawDuration)/ 1000000000; // Convert to milliseconds;
    return success;
}

bool StorageAudioSource::queryPosition(qint64 &positionNs) const {
    if (!m_player) return false;
    gint64 rawPosition = 0;
    bool success = gst_element_query_position(m_player, GST_FORMAT_TIME, &rawPosition);
    positionNs = static_cast<qint64>(rawPosition)/ 1000000000; // Convert to milliseconds;
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

QStringList StorageAudioSource::list() const {
    QStringList list;
    for (const QString &song : m_songs) {
        QFileInfo info(song);
        if (info.isFile()){
            list << info.baseName(); // just filename without path or extension
        }
        else {
            list << song + " (folder)"; // treat directories as is
        }
    }
    return list;
}

void StorageAudioSource::playAt(int index) {
    if (index >= 0 && index < m_songs.size()) {
        currentIndex = index;
        play();
    }
}

bool StorageAudioSource::importFromUsb()
{
    QString usbPath = detectUsbPath();
    if (usbPath.isEmpty()) return false;

    QString targetDir = QStandardPaths::writableLocation(QStandardPaths::MusicLocation);
    QDirIterator it(usbPath, QDirIterator::Subdirectories);
    QStringList copied;

    while (it.hasNext()) {
        QString filePath = it.next();
        qDebug() << "[StorageAudioSource] Found file:" << filePath;
        if (!isMediaFile(filePath)) continue;

        // Preserve relative path from usb root
        QString relativePath = QDir(usbPath).relativeFilePath(filePath);
        QString destPath = targetDir + "/" + relativePath;
        qDebug() << "[StorageAudioSource] Copying to:" << destPath;

        // Ensure destination directory exists
        QFileInfo destInfo(destPath);
        QDir().mkpath(destInfo.absolutePath());
        
        bool shouldCopy = true;
        if (QFile::exists(destPath)) {
            QFileInfo sourceInfo(filePath);
            QFileInfo destInfo(destPath);
        
            if (sourceInfo.size() == destInfo.size()) {
                qDebug() << "[StorageAudioSource] Skipped (already exists and size matches):" << destPath;
                shouldCopy = false;
            }
        }
        
        if (shouldCopy) {
            if (QFile::copy(filePath, destPath)) {
                qDebug() << "[StorageAudioSource] Copied:" << filePath << "→" << destPath;
                copied << destPath;
            } else {
                qWarning() << "[StorageAudioSource] Failed to copy:" << filePath << "→" << destPath;
            }
        }
    }

    if (copied.isEmpty()) {
        qWarning() << "[StorageAudioSource] No valid media files found on USB.";
        return false;
    }

    m_songs = copied;
    emit songListChanged();
    scanLocalMusic(QStandardPaths::writableLocation(QStandardPaths::MusicLocation));
    return true;
}

QString StorageAudioSource::detectUsbPath() const
{
    QString basePath = "/mnt/usb";
    QDir baseDir(basePath);
    QStringList subDirs = baseDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    if (!subDirs.isEmpty()) {
        return basePath;
    }
    return QString();
}

bool StorageAudioSource::isMediaFile(const QString &filePath) const
{
    QStringList validExtensions = { "mp3", "wav", "flac", "ogg", "aac" };
    QFileInfo fileInfo(filePath);
    return validExtensions.contains(fileInfo.suffix().toLower());
}

