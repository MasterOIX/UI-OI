import QtQuick 2.12
import QtQuick.Window 2.12
import "UI/BottomBar"
import "UI/RightScreen"
import "UI/LeftScreen"
import "UI/TopBar"

Window {
    width: 1024
    height: 600
    visible: true
    // flags: Qt.FramelessWindowHint
    title: qsTr("Infotainment")

    BottomBar {
        id: bottomBar
    }

    RightScreen {
        id: rightScreen
    }

    LeftScreen {
        id: leftScreen
    }

    TopBar {
        id: topBar
    }


}
