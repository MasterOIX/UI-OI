#ifndef BLUETOOTHAUDIOSOURCE_H
#define BLUETOOTHAUDIOSOURCE_H

#include "audiosource.h"
#include "qdbusextratypes.h"
#include <QVariantMap>
#include <QObject>
#include <QTimer>
#include <gst/gst.h>
#include <gst/gstpipeline.h>

class BluetoothAudioSource : public AudioSource {
    Q_OBJECT
public:
    explicit BluetoothAudioSource(QObject *parent = nullptr);

    void next() override;
    void previous() override;
    void play() override;
    void stop() override;
    void pause() override;

    bool queryDuration(qint64 &durationMs) const override;
    bool queryPosition(qint64 &positionMs) const override;

    QString title() const override;
    QString artist() const override;
    QString album() const override;
    QStringList list() const override { return {}; }
    void playAt(int index) override;
    void setVolume(int volumePercent) override;
    void setBluezDevicePath(const QString &path);
    void callControlMethod(const QString &method);
    void getTrack();

private slots:
    void onPropertiesChanged(const QString &interface, const QVariantMap &changedProperties, const QStringList &invalidatedProperties);
    void onInterfacesAdded(const QDBusObjectPath &objectPath, const QVariantMap &interfaces);
    void onInterfacesRemoved(const QString &objectPath, const QStringList &interfaces);
    void handleStreamRestart();

private:

    QString m_bluezDevicePath;   // e.g. "/org/bluez/hci0/dev_XX_XX_XX_XX_XX_XX"
    QString m_playerPath;        // e.g. "/org/bluez/hci0/dev_XX_XX_XX_XX_XX_XX/player0"
    QString m_service = "org.bluez";
    QVariantMap m_currentTrack;
    GstElement *m_gstPipeline = nullptr;
    GstElement *m_volumeElement = nullptr;
};

#endif // BLUETOOTHAUDIOSOURCE_H
