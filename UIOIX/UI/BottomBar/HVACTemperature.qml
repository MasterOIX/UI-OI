import QtQuick 2.12
import HVAC 1.0


Rectangle {
    id: hvacTemperature

    property var zoneModel

    anchors {
        top: parent.top
        bottom: parent.bottom
    }

    color: "transparent"
    width: 90

    Rectangle {
        id: decrementButton
        anchors {
            left: parent.left
            top: parent.top
            bottom: parent.bottom
        }
        width: height / 2
        color: "transparent"

        Text {
            id: decrementText
            anchors.centerIn: parent
            text: qsTr("<")
            color: "white"
            font.pixelSize: 16
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                zoneModel.increaseTemperature(-1)
                if(zoneModel === hvacHandler.driverZone) {
                    if(hvacHandler.syncEnabled) {
                        hvacHandler.passengerZone.temperature = hvacHandler.driverZone.temperature
                    }
                }
                else {
                    hvacHandler.syncEnabled = false
                }
            }
            cursorShape: Qt.PointingHandCursor
        }
    }

    Rectangle {
        id: temperature
        anchors {
            left: decrementButton.right
            top: parent.top
            bottom: parent.bottom
        }
        color: "transparent"

        width: height / 3 * 2
        height: parent.height
        Text {
            id: temperatureText
            text: zoneModel.temperature
            color: "white"
            font.pixelSize: 20
            anchors.centerIn: parent
        }
    }

    Rectangle {
        id: incrementButton
        anchors {
            left: temperature.right
            top: parent.top
            bottom: parent.bottom
        }
        color: "transparent"

        width: height / 2
        Text {
            id: incrementText
            anchors.centerIn: parent
            text: qsTr(">")
            color: "white"
            font.pixelSize: 16
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                zoneModel.increaseTemperature(1)
                /*if(zoneModel === hvacHandler.driverZone) {
                    if(hvacHandler.syncEnabled) {
                        hvacHandler.passengerZone.temperature = hvacHandler.driverZone.temperature
                    }
                }
                else {
                    hvacHandler.syncEnabled = false
                }*/
            }
            cursorShape: Qt.PointingHandCursor
        }
    }
}
