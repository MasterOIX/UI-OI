#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include <QLocale>
#include <QTranslator>

#include <QSslSocket>
#include <QDebug>
#include <QtDBus/QDBusMetaType>
#include <QtDBus/QDBusObjectPath>
#include <QMap>
#include <QVariantMap>


#include <Controllers/system.h>
#include <Controllers/hvachandler.h>
#include <Controllers/audiocontroller.h>
#include <Controllers/zonecontroller.h>
#include <Controllers/valhallacontroller.h>
#include <Controllers/wificontroller.h>
#include <Controllers/bluetoothcontroller.h>

QDBusArgument &operator<<(QDBusArgument &argument, const QMap<QString, QVariantMap> &map)
{
    argument.beginMap(qMetaTypeId<QString>(), qMetaTypeId<QVariantMap>());
    for (auto it = map.begin(); it != map.end(); ++it) {
        argument.beginMapEntry();
        argument << it.key() << it.value();
        argument.endMapEntry();
    }
    argument.endMap();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, QMap<QString, QVariantMap> &map)
{
    map.clear();
    argument.beginMap();
    while (!argument.atEnd()) {
        QString key;
        QVariantMap value;
        argument.beginMapEntry();
        argument >> key >> value;
        argument.endMapEntry();
        map.insert(key, value);
    }
    argument.endMap();
    return argument;
}

// For QMap<QDBusObjectPath, QMap<QString, QVariantMap>>
QDBusArgument &operator<<(QDBusArgument &argument, const QMap<QDBusObjectPath, QMap<QString, QVariantMap>> &map)
{
    argument.beginMap(qMetaTypeId<QDBusObjectPath>(), qMetaTypeId<QMap<QString, QVariantMap>>());
    for (auto it = map.begin(); it != map.end(); ++it) {
        argument.beginMapEntry();
        argument << it.key() << it.value();
        argument.endMapEntry();
    }
    argument.endMap();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, QMap<QDBusObjectPath, QMap<QString, QVariantMap>> &map)
{
    map.clear();
    argument.beginMap();
    while (!argument.atEnd()) {
        QDBusObjectPath key;
        QMap<QString, QVariantMap> value;
        argument.beginMapEntry();
        argument >> key >> value;
        argument.endMapEntry();
        map.insert(key, value);
    }
    argument.endMap();
    return argument;
}

int main(int argc, char *argv[])
{
    qDBusRegisterMetaType<QMap<QString, QVariantMap>>();
    qDBusRegisterMetaType<QMap<QDBusObjectPath, QMap<QString, QVariantMap>>>();

    if (qEnvironmentVariableIsEmpty("QTGLESSTREAM_DISPLAY")) {
        qputenv("QT_QPA_EGLFS_PHYSICAL_WIDTH", QByteArray("213"));
        qputenv("QT_QPA_EGLFS_PHYSICAL_HEIGHT", QByteArray("120"));

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    }

    QGuiApplication app(argc, argv);

    qmlRegisterUncreatableType<ZoneController>("HVAC", 1, 0, "ZoneController", "Accessed via HVACHandler");
    qmlRegisterUncreatableType<AudioController>("Audio", 1, 0, "AudioController", "Enum only");
    qmlRegisterType<ValhallaController>("Controllers", 1, 0, "ValhallaController");


    System m_system_handler;
    HVACHandler m_hvac_handler;
    BluetoothController m_bluetooth_controller;
    AudioController m_audio_controller(&m_bluetooth_controller);
    ValhallaController m_valhalla_controller;
    WiFiController m_wifi_controller;

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "UIOI_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            app.installTranslator(&translator);
            break;
        }
    }

    QQmlApplicationEngine engine;

    QQmlContext *context = engine.rootContext();
    context->setContextProperty("systemHandler", &m_system_handler);
    context->setContextProperty("hvacHandler", &m_hvac_handler);
    context->setContextProperty("audioController", &m_audio_controller);
    context->setContextProperty("valhalla_controller", &m_valhalla_controller);
    context->setContextProperty("wifiController", &m_wifi_controller);
    context->setContextProperty("bluetoothController", &m_bluetooth_controller);

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreated,
        &app,
        [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);
    engine.load(url);
    return app.exec();
}
