import QtQuick 2.12
import QtQuick.Window 2.12
import "UI/BottomBar"
import "UI/RightScreen"
import "UI/LeftScreen"
import "UI/TopBar"
import "UI/Menu"

Window {
    width: 1024
    height: 600
    visible: true
    // flags: Qt.FramelessWindowHint
    title: qsTr("Infotainment")



    RightScreen {
        id: rightScreen
    }

    LeftScreen {
        id: leftScreen
    }

    Menu {
        visible: false
        id: menu
    }

    TopBar {
        id: topBar
    }

    BottomBar {
        id: bottomBar
    }

}
