import QtQuick 2.12

Rectangle {
    id: volumeControl
    anchors {
        top: parent.top
        bottom: parent.bottom
        right: parent.right
    }
    color: "transparent"

    width: 120

    function showVolumeTextTemporarily() {
        textRect.visible = true
        volumeIcon.visible = false
        volumeDisplayTimer.restart()
    }

    Timer {
        id: volumeDisplayTimer
        interval: 1000
        repeat: false
        running: false
        onTriggered: {
            textRect.visible = false
            volumeIcon.visible = true
        }
    }

    Image {
        id: plusIcon
        anchors {
            right: parent.right
            rightMargin: 20
            verticalCenter: parent.verticalCenter
        }
        height: parent.height * 0.30
        fillMode: Image.PreserveAspectFit
        source: "qrc:/UI/assests/plus.png"
        MouseArea {
            anchors.fill: parent
            onClicked: {
                audioController.increaseVolume(1)
                showVolumeTextTemporarily()
            }
            cursorShape: Qt.PointingHandCursor
            onPressAndHold: {
                volumeTimer.start()
            }
            onReleased: {
                if(volumeTimer.running) {
                    volumeTimer.stop()
                }
            }
        }

        Timer {
            id: volumeTimer
            interval: 200
            repeat: true
            running: false
            onTriggered: {
                audioController.increaseVolume(5)
                showVolumeTextTemporarily()
            }
        }
    }

    Image {
        id: volumeIcon
        visible: true
        anchors {
            right: plusIcon.left
            rightMargin: 10
            verticalCenter: parent.verticalCenter
        }
        smooth: true
        width: 32
        height: 32
        fillMode: Image.PreserveAspectFit
        source: audioController.volume > 50 ? "qrc:/UI/assests/volume_1.png"
              : audioController.volume > 0 ? "qrc:/UI/assests/volume_2.png"
              : "qrc:/UI/assests/mute.png"
        MouseArea {
            anchors.fill: parent
            onClicked: {
                audioController.increaseVolume( -audioController.volume)
            }
        }
    }

    Rectangle {
        id: textRect
        visible: false
        anchors {
            right: plusIcon.left
            rightMargin: 10
            verticalCenter: parent.verticalCenter
        }
        color: "transparent"
        width: 32
        height: 32
        Text {
            id: volumeText
            anchors.centerIn: parent
            text: audioController.volume

            color: "white"
            font.pixelSize: 20
            font.bold: true
        }
    }

    Image {
        id: minusIcon
        anchors {
            right: volumeIcon.left
            rightMargin: 12
            verticalCenter: parent.verticalCenter
        }
        width: parent.height * 0.3
        fillMode: Image.PreserveAspectFit
        source: "qrc:/UI/assests/minus.png"
        MouseArea {
            anchors.fill: parent
            onClicked: {
                audioController.increaseVolume(-1)
                showVolumeTextTemporarily()
            }
            onPressAndHold: {
                volumeTimer1.start()
            }
            onReleased: {
                if(volumeTimer1.running) {
                    volumeTimer1.stop()
                }
            }
            cursorShape: Qt.PointingHandCursor
        }

        Timer {
            id: volumeTimer1
            interval: 200
            repeat: true
            running: false
            onTriggered: {
                audioController.increaseVolume(-5)
                showVolumeTextTemporarily()
            }
        }
    }
}
