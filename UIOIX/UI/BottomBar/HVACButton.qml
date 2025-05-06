import QtQuick 2.12

Rectangle {
    property alias buttonTxt: buttonText.text
    property bool value: false
    property var toggleFunction

    id: buttonIcon

    width: parent.height
    color: "black"

    MouseArea {
        id: buttonArea
        anchors.fill: parent
        cursorShape: Qt.PointingHandCursor
        onClicked: {
            value = !value
            if (toggleFunction) {
                toggleFunction(value)
            }
        }
    }

    Text {
        id: buttonText
        anchors {
            centerIn: parent
        }

        color: "white"
        font.pixelSize: 16
        font.bold: true
    }

    Rectangle {
        id: buttonLedRect
        anchors {
            top: parent.top
            topMargin: parent.height / 2
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        color: "transparent"

        Rectangle {
            id: buttonLed
            width: buttonLedRect.width / 3
            height: buttonLedRect.height / 5
            anchors.centerIn: parent
            color: buttonIcon.value ? "#39FF14" : "#515152"
            radius: 5
            border.color: "white"
            border.width: 1
        }
    }
}
