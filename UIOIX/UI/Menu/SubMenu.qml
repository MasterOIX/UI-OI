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

    Item {
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

        DeviceConnections {
            id: deviceConnections
            visible: menuList.selectedItem === "Connectivity"
            anchors.fill: parent
        }

        Media {
            id: media
            visible: menuList.selectedItem === "Media"
            anchors.fill: parent
        }

        Settings {
            id: settings
            visible: menuList.selectedItem === "Settings"
            anchors.fill: parent
        }

        TripComputer {
            id: tripComputer
            visible: menuList.selectedItem === "Trip Computer"
            anchors.fill: parent
        }

        VehicleStatus {
            id: vehicleStatus
            visible: menuList.selectedItem === "Vehicle Status"
            anchors.fill: parent
        }
    }
}
