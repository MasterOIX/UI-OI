import QtQuick 2.12

Item {
    id: seatHeater
    property var zone  // expects driverZone or passengerZone
    width: 40
    height: 40

    Image {
        id: seatImage
        source: zone === hvacHandler.passengerZone
                ? "qrc:/UI/assests/seat_p.png"
                : "qrc:/UI/assests/seat.png"
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        height: 20
        fillMode: Image.PreserveAspectFit

        MouseArea {
            anchors {
                top: parent.top
                topMargin: -20
                bottom: parent.bottom
                bottomMargin: -30
                left: parent.left
                leftMargin: -15
                right: parent.right
                rightMargin: -15
            }

            onClicked: {
                zone.increaseSeatHeating()
            }
            cursorShape: Qt.PointingHandCursor
        }
    }

    Row {
        id: seatDots
        spacing: 6
        anchors.top: seatImage.bottom
        anchors.topMargin: 4
        anchors.horizontalCenter: parent.horizontalCenter

        Repeater {
            model: 3
            delegate: Rectangle {
                width: 6
                height: 6
                radius: 3
                color: index < zone.seatHeating ? "#39FF14" : "#515152"
                border.color: "white"
                border.width: 1
            }
        }
    }
}
