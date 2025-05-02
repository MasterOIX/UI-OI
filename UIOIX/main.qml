import QtQuick 2.12
import QtQuick.Window 2.12
import "UI/BottomBar"

Window {
    width: 1024
    height: 600
    visible: true
    title: qsTr("Infotainment")

    BottomBar {
        id: bottomBar
    }
}
