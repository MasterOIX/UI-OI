import QtQuick 2.12
import QtQuick3D 1.15

Rectangle {
    id: leftScreen

    anchors {
        top: parent.top
        bottom: bottomBar.top
        left: parent.left
        right: rightScreen.left
    }
    color: "white"
}
