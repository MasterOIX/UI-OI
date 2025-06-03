QT += quick qml quickcontrols2 location positioning opengl network core dbus # texttospeech

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        Controllers/audiocontroller.cpp \
        Controllers/audiosourcemanager.cpp \
        Controllers/bluetoothaudiosource.cpp \
        Controllers/bluetoothcontroller.cpp \
        Controllers/hvachandler.cpp \
        Controllers/onlineradioaudiosource.cpp \
        Controllers/radioaudiosource.cpp \
        Controllers/settingsmanager.cpp \
        Controllers/storageaudiosource.cpp \
        Controllers/system.cpp \
        Controllers/valhallacontroller.cpp \
        Controllers/wificontroller.cpp \
        Controllers/wifinetwork.cpp \
        Controllers/zonecontroller.cpp \
        main.cpp

RESOURCES += qml.qrc
LIBS += -lasound
LIBS += -lm

TRANSLATIONS += \
    UIOI_ro_RO.ts
CONFIG += lrelease
CONFIG += embed_translations
CONFIG += c++17
CONFIG += link_pkgconfig

PKGCONFIG += \
    gstreamer-1.0 \
    gstreamer-base-1.0

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    Controllers/audiocontroller.h \
    Controllers/audiosource.h \
    Controllers/audiosourcemanager.h \
    Controllers/bluetoothaudiosource.h \
    Controllers/bluetoothcontroller.h \
    Controllers/bluezagent.h \
    Controllers/hvachandler.h \
    Controllers/onlineradioaudiosource.h \
    Controllers/radioaudiosource.h \
    Controllers/settingsmanager.h \
    Controllers/storageaudiosource.h \
    Controllers/system.h \
    Controllers/valhallacontroller.h \
    Controllers/wificontroller.h \
    Controllers/wifinetwork.h \
    Controllers/zonecontroller.h
