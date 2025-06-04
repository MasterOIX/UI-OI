import QtQuick 2.15
import QtGraphicalEffects 1.15

Rectangle {
    id: subMenu
    anchors {
        top: parent.top
        bottom: parent.bottom
        right: parent.right
    }
    width: parent.width * 9 / 15
    color: "#2d2d2d"

    Text {
        id: title
        text: menuList.selectedItem
        anchors {
            top: parent.top
            left: parent.left
            topMargin: 40
            leftMargin: 40
        }
        color: "white"
        font.pixelSize: 24
        font.bold: true
    }

    Loader {
        id: pageLoader
        anchors {
            top: title.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
            topMargin: 20
            leftMargin: 40
            rightMargin: 40
            bottomMargin: 40
        }

        source: {
            switch (menuList.selectedItem) {
            case "Connections": return "DeviceConnections.qml";
            case "Media": return "Media.qml";
            case "Settings": return "Settings.qml";
            case "Trip Computer": return "TripComputer.qml";
            case "Vehicle Status": return "VehicleStatus.qml";
            default: return "";
            }
        }
    }
}
