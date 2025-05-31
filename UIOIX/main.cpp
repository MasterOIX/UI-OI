#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include <QLocale>
#include <QTranslator>

#include <QSslSocket>
#include <QDebug>

#include <Controllers/system.h>
#include <Controllers/hvachandler.h>
#include <Controllers/audiocontroller.h>
#include <Controllers/zonecontroller.h>
#include <Controllers/valhallacontroller.h>

int main(int argc, char *argv[])
{
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
    AudioController m_audio_controller;
    ValhallaController m_valhalla_controller;

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
