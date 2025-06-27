import QtQuick 2.12
import QtGraphicalEffects 1.0

Item {
    id: fanSpeedSelector
    height: 100
    anchors {
        left: driverHVACTemperature.right
        leftMargin: 40
        top: parent.top
        right: passengerHVACTemperature.left
        rightMargin: 40
    }

    property int selectedSpeed: 0  // 0 = OFF, 1–5 = levels, 99 = MAX

    // 🔹 Little Fan (OFF)
    Image {
        id: littleFan
        source: "qrc:/UI/assets/radiator-fan32.png"
        width: 24
        height: 24
        fillMode: Image.PreserveAspectFit
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 0
        anchors.topMargin: 35

        MouseArea {
            anchors.fill: parent
            onClicked: selectedSpeed = 0
            cursorShape: Qt.PointingHandCursor

            onPressed: {
                hvacHandler.setSpeedFan(0);
            }
        }
    }

    Text {
        text: "OFF"
        anchors.top: littleFan.bottom
        anchors.topMargin: 8
        anchors.horizontalCenter: littleFan.horizontalCenter
        color: "white"
        font.pixelSize: 10
        font.bold: true
    }

    // 🔸 Fan Speed Dots (1–5)
    Item {
        id: fanDots
        width: 50 * 5
        height: 50
        anchors {
            left: littleFan.right
            leftMargin: 20
            verticalCenter: littleFan.verticalCenter
        }

        Repeater {
            id: dotRepeater
            model: 5
            delegate: Item {
                width: 50
                height: 50
                x: index * 50

                Rectangle {
                    width: selectedSpeed === index + 1 ? 24 : 6
                    height: selectedSpeed === index + 1 ? 24 : 6
                    radius: width / 2
                    color: "white"
                    anchors.centerIn: parent
                    border.width: 1
                    border.color: "#888"

                    Text {
                        text: selectedSpeed === index + 1 ? (index + 1) : ""
                        color: "black"
                        anchors.centerIn: parent
                        font.pixelSize: 14
                        font.bold: true
                    }

                    Behavior on width { NumberAnimation { duration: 150 } }
                    Behavior on height { NumberAnimation { duration: 150 } }
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        selectedSpeed = (selectedSpeed === index + 1) ? 0 : index + 1
                        hvacHandler.setSpeedFan(selectedSpeed);
                    }
                    cursorShape: Qt.PointingHandCursor
                }
            }
        }
    }

    // 🔹 Big Fan (MAX)
    Rectangle {
        id: bigFanContainer
        width: 32
        height: 32
        anchors.right: parent.right
        anchors.verticalCenter: littleFan.verticalCenter
        anchors.rightMargin: 0
        color: "transparent"

        Image {
            id: bigFan
            source: "qrc:/UI/assets/radiator-fan.png"
            anchors.fill: parent
            fillMode: Image.PreserveAspectFit

            property real angleValue: 0

            transform: Rotation {
                origin.x: bigFan.width / 2
                origin.y: bigFan.height / 2
                angle: bigFan.angleValue
            }

            NumberAnimation on angleValue {
                from: 0
                to: 360
                duration: 1500
                loops: Animation.Infinite
                running: selectedSpeed === 99
            }
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                selectedSpeed = (selectedSpeed === 99 ? 0 : 99)
                hvacHandler.setSpeedFan(selectedSpeed);
            }
            cursorShape: Qt.PointingHandCursor
        }
    }

    Text {
        text: "MAX"
        anchors.top: littleFan.bottom
        anchors.topMargin: 8
        anchors.horizontalCenter: bigFanContainer.horizontalCenter
        color: "white"
        font.pixelSize: 10
        font.bold: true
    }
}
