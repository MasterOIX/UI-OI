import QtQuick 2.12


Rectangle {
    id: bottomBar
    anchors {
        bottom: parent.bottom
        left: parent.left
        right: parent.right
    }
    height: parent.height / 11
    color: "black"

    Image {
        id: carSettingsIcon
        anchors {
            left: parent.left
            leftMargin: 30
            verticalCenter: parent.verticalCenter

        }
        height: parent.height * 0.65
        fillMode: Image.PreserveAspectFit
        source: "qrc:/UI/assests/car_front_icon.png"
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
                console.log("Image clicked")
                systemHandler.increaseVolume()
            }
            cursorShape: Qt.PointingHandCursor
        }
    }

    Image {
        id: volumeIcon
        anchors {
            right: plusIcon.left
            rightMargin: 10
            verticalCenter: parent.verticalCenter
        }
        smooth: true
        height: parent.height * 0.55
        fillMode: Image.PreserveAspectFit
        source: systemHandler.volume > 50 ? "qrc:/UI/assests/volume_1.png"
              : systemHandler.volume > 0 ? "qrc:/UI/assests/volume_2.png"
              : "qrc:/UI/assests/mute.png"
    }

    Image {
        id: minusIcon
        anchors {
            right: volumeIcon.left
            rightMargin: 12
            verticalCenter: parent.verticalCenter
        }
        width: parent.height * 0.30
        fillMode: Image.PreserveAspectFit
        source: "qrc:/UI/assests/minus.png"
        MouseArea {
            anchors.fill: parent
            onClicked: {
                console.log("Image clicked")
                systemHandler.decreaseVolume()
            }
            cursorShape: Qt.PointingHandCursor
        }
    }
}
