import QtQuick 2.12
import QtGraphicalEffects 1.0

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
        }

        Image {
            id: pausePlayIcon
            source: "qrc:/UI/assests/pause.png"
            anchors {
                left: previousIcon.right
                leftMargin: 55
                verticalCenter: parent.verticalCenter
            }
            width: parent.height * 0.2
            height: width
            fillMode: Image.PreserveAspectFit
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
        }
    }
}
