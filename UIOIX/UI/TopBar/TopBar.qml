import QtQuick 2.12

Rectangle {
    id: topBar
    anchors{
        top: parent.top
        left: parent.left
        right: parent.right
    }
    height: parent.height / 25
    color: Qt.rgba(0.827, 0.827, 0.827, 0.5)

    property var roLocale: Qt.locale("ro_RO")
    Text {
        id: clockText
        anchors {
            centerIn: parent
        }
        color: "black"

        font.pixelSize: 14
        font.bold:true
        text: new Date().toLocaleTimeString(roLocale, "hh:mm")
    }

    Timer {
        interval: 1000  // 1 second
        running: true
        repeat: true
        onTriggered: clockText.text = new Date().toLocaleTimeString(roLocale, "hh:mm")
    }

    Image {
        id: lockIcon
        anchors {
            verticalCenter: clockText.verticalCenter
            right: clockText.left
            rightMargin: 60
        }
        width: parent.width / 60
        fillMode: Image.PreserveAspectFit

        source: {
            systemHandler.carLocked ? "qrc:/UI/assests/lock.png" : "qrc:/UI/assests/unlock.png"
        }
        MouseArea {
            anchors.fill: parent
            onClicked: {
                systemHandler.carLocked = !systemHandler.carLocked
            }
        }
    }
}
