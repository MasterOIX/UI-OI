import QtQuick 2.12
import HVAC 1.0


Rectangle {
    id: hvacTemperature

    property var zoneModel
    property real startY: 0

    color: "transparent"
    width: 90
    height: bar_height

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

        MouseArea {
            anchors.fill: parent
            onPressed: {
                startY = mouse.y
            }

            onReleased: {
                var deltaY = startY - mouse.y
                if (deltaY > 50) { // threshold for swipe up
                    if (bottomBar.height === bar_height) {
                        bottomBar.height = bottomBar.height * 2
                        collapseTimer.start()
                    }
                }
                else if (deltaY < -50) { // threshold for swipe down
                    if (bottomBar.height !== bar_height) {
                        bottomBar.height = bottomBar.height / 2
                        collapseTimer.stop()
                    }
                }
            }
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
}
