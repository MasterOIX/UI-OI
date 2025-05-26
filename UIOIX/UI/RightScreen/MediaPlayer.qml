import QtQuick 2.12
import QtGraphicalEffects 1.0
import Audio 1.0

Item {
    anchors {
        bottom: parent.bottom
        left: parent.left
        right: parent.right
        leftMargin: 10
        rightMargin: 10
    }

    height: 90

    Rectangle {
        id: baseRect
        anchors.fill: parent
        color: Qt.rgba(0.94, 0.94, 0.94, 0.7)
    }

    DropShadow {
        anchors.fill: baseRect
        source: baseRect
        horizontalOffset: 0
        verticalOffset: 3
        radius: 8
        samples: 20
        color: "#33000000"  // semi-transparent black
    }

    Rectangle {
        id: imageRect
        anchors {
            left: parent.left
            leftMargin: 5
            top: parent.top
            topMargin: 5
            bottom: parent.bottom
            bottomMargin: 5
        }
        width: height
        Image {
            id: mediaPhoto
            source: "qrc:/UI/assests/media.jpg"
            anchors.fill: parent
            fillMode: Image.PreserveAspectFit
        }
    }

    Rectangle {
        id: loadingRect
        anchors {
            left: imageRect.right
            leftMargin: 8
            right: controlsRect.left
            rightMargin: 8
            bottom: parent.bottom
            bottomMargin: 10
        }
        height: 3
        color: "lightgray"
        radius: 2

        Rectangle {
            id: loadingProgress
            anchors {
                left: parent.left
                verticalCenter: parent.verticalCenter
            }
            color: "gray"
            radius: 2
            height: 4
            width: loadingRect.width * (audioController.duration > 0 ? audioController.position / audioController.duration : 0)

            Behavior on width {
                NumberAnimation {
                    duration: 150
                    easing.type: Easing.InOutQuad
                }
            }
        }

        DropShadow {
            anchors.fill: loadingProgress
            source: loadingProgress
            horizontalOffset: 0
            verticalOffset: 1
            radius: 3
            samples: 12
            color: "#88000000"
        }
    }


    Text {
        id: timeLeft
        text: {
            const remaining = Math.max(0, audioController.duration - audioController.position);
            const minutes = Math.floor(remaining / 60);
            const seconds = Math.floor(remaining % 60);
            return `-${minutes}:${seconds < 10 ? "0" + seconds : seconds}`;
        }
        anchors {
            right: loadingRect.right
            bottom: loadingRect.top
            bottomMargin: 3
        }
        color: "gray"
        font.pixelSize: 8
    }


    Item {
        id: songContainer
        anchors {
            left: imageRect.right
            right: controlsRect.left
            leftMargin: 8
            rightMargin: 8
            top: parent.top
            topMargin: 10
        }
        height: 22
        clip: true

        Item {
            id: scrollingGroup
            width: text1.width + spacer + text2.width
            height: parent.height

            property int spacer: 60  // spacing between repeats
            x: 0

            SequentialAnimation on x {
                id: scrollAnim
                running: true
                loops: Animation.Infinite
                NumberAnimation {
                    from: 0
                    to: -(text1.width + scrollingGroup.spacer)
                    duration: 12000
                    easing.type: Easing.Linear
                }
            }

            Text {
                id: text1
                text: audioController.currentTitle
                font.pixelSize: 18
                font.bold: true
                font.family: "Arial"
                color: "black"
                anchors.verticalCenter: parent.verticalCenter
            }

            Text {
                id: text2
                text: audioController.currentTitle
                font.pixelSize: 18
                font.bold: true
                font.family: "Arial"
                color: "black"
                anchors.verticalCenter: parent.verticalCenter
                x: text1.width + scrollingGroup.spacer
            }

            Connections {
                target: audioController
                function onMetadataChanged() {
                    scrollAnim.stop()
                    scrollingGroup.x = 0
                    scrollAnim.start()
                }
            }
        }
    }

    Text {
        id: artistName
        anchors {
            left: imageRect.right
            leftMargin: 8
            top: songContainer.bottom
            topMargin: 5
        }
        color: "black"
        text: audioController.currentArtist
        font.pixelSize: 14
        font.bold: true
        font.family: "Arial"
    }

    Text {
        id: albumName
        anchors {
            left: imageRect.right
            leftMargin: 8
            top: artistName.bottom
            topMargin: 5
        }
        text: audioController.currentAlbum
        font.pixelSize: 12
        font.family: "Arial"
        color: "gray"
    }

    Rectangle {
        id: controlsRect
        anchors {
            right: parent.right
            top: parent.top
            bottom: parent.bottom
        }
        width: parent.width / 2
        color: "transparent"
        Image {
            id: previousIcon
            source: "qrc:/UI/assests/previous.png"
            anchors {
                left: parent.left
                verticalCenter: parent.verticalCenter
            }
            width: parent.height * 0.2
            height: width
            fillMode: Image.PreserveAspectFit
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    audioController.previous()
                }
            }
        }

        Image {
            id: pausePlayIcon
            source: audioController.isPlaying ? "qrc:/UI/assests/pause.png" : "qrc:/UI/assests/play.png"
            anchors {
                left: previousIcon.right
                leftMargin: 55
                verticalCenter: parent.verticalCenter
            }
            width: parent.height * 0.2
            height: width
            fillMode: Image.PreserveAspectFit
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    audioController.togglePlayPause()
                }
            }
        }

        Image {
            id: nextIcon
            source: "qrc:/UI/assests/next.png"
            anchors {
                left: pausePlayIcon.right
                leftMargin: 55
                verticalCenter: parent.verticalCenter
            }
            width: parent.height * 0.2
            height: width
            fillMode: Image.PreserveAspectFit

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    audioController.next()
                }
            }
        }

        Image {
            id: modeIcon
            source: "qrc:/UI/assests/mode.png"
            anchors {
                left: nextIcon.right
                leftMargin: 55
                verticalCenter: parent.verticalCenter
            }
            width: parent.height * 0.3
            height: width
            fillMode: Image.PreserveAspectFit
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    audioController.toggleMode()
                }
            }
        }

        Image {
            id: modeSelectedIcon
            source: audioController.mode === AudioController.Bluetooth ? "qrc:/UI/assests/bluetooth.png" :
                    audioController.mode === AudioController.Radio ? "qrc:/UI/assests/fm.png" :
                    audioController.mode === AudioController.Storage ? "qrc:/UI/assests/storage.png":
                    "qrc:/UI/assests/webradio.png"
            anchors {
                right: parent.right
                top: parent.top
                rightMargin: 5
                topMargin: 5
            }
            width: parent.height * 0.15
            height: width
            fillMode: Image.PreserveAspectFit
        }
    }
}
