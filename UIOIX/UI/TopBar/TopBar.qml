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

    Rectangle {
        id: battery
        anchors {
            verticalCenter: clockText.verticalCenter
            right: lockIcon.left
            rightMargin: 20
        }
        height: parent.height * 0.60
        width: 30
        color: Qt.rgba(0.827, 0.827, 0.827, 0.0)
        radius: 3

        Rectangle {
            id: batteryLevel
            anchors {
                left: parent.left
                top: parent.top
                bottom: parent.bottom
                leftMargin: 1
                topMargin: 1
                bottomMargin: 1
            }
            color: systemHandler.batteryPercent > 50 ? "green"
                 : systemHandler.batteryPercent > 20 ? "yellow"
                 : "red"
            width: systemHandler.batteryPercent > 10 ? systemHandler.batteryPercent * parent.width / 100 : 10 * parent.width / 100
            radius: 3
            z: 0
        }

        Timer {
            interval: 1000  // 1 second
            repeat: true
            running: true

            onTriggered: {
                if (systemHandler.batteryPercent <= 10) {
                    if(batteryLevel.color == "#ff0000") {
                        batteryLevel.color = "transparent"
                    }
                    else {
                        batteryLevel.color = "red"
                    }
                }
            }
        }

        Rectangle{
            id: batteryBorder
            anchors.fill: parent
            color: "transparent"
            border.color: "black"
            border.width: 1
            radius: 3
            z: 1
        }
        Text {
            id: batteryLevelText
            anchors.centerIn: parent
            color: "black"
            font.pixelSize: 10
            text: systemHandler.batteryPercent + "%"
        }
    }

    Rectangle {
        id: batteryPin
        anchors {
            left: battery.right
            verticalCenter: battery.verticalCenter
            leftMargin: 1
        }
        color: "black"
        width: 2
        height: 5
        radius: 3
    }

    Text {
        id: temperature
        anchors {
            verticalCenter: clockText.verticalCenter
            left: clockText.right
            leftMargin: 20
        }
        color: "black"
        font.pixelSize: 14
        font.bold:true
        text: systemHandler.outdoorTemp + "°C"
    }

    Image {
        id: settingsIcon
        anchors {
            verticalCenter: clockText.verticalCenter
            right: clockText.left
            rightMargin: 20
        }
        source: "qrc:/UI/assests/settings.png"
        width: parent.width / 60
        fillMode: Image.PreserveAspectFit
    }

    Text {
        id: userText
        text: systemHandler.userName.length > 12 ? systemHandler.userName.substring(0, 12) + "..."
            : systemHandler.userName.length > 0 ? systemHandler.userName
            : "User"
        anchors {
            verticalCenter: clockText.verticalCenter
            right: parent.right
            rightMargin: 20
        }
        color: "black"
        font.pixelSize: 14
        font.bold:true
    }

    Image {
        id: userIcon
        anchors {
            verticalCenter: clockText.verticalCenter
            right: userText.left
            rightMargin: 10
        }
        width: parent.width / 60
        fillMode: Image.PreserveAspectFit
        source: "qrc:/UI/assests/user.png"
    }
}
