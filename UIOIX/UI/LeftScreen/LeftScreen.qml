import QtQuick 2.12

Rectangle {
    id: leftScreen

    anchors {
        top: parent.top
        bottom: bottomBar.top
        left: parent.left
        right: rightScreen.left
    }

    Rectangle {
        anchors.centerIn: parent
        width: parent.width * 0.6
        height: parent.height * 0.6


        Image {
            id: carImg
            source: "qrc:/UI/assests/carimg.png"
            anchors.fill: parent
            fillMode: Image.PreserveAspectFit
        }
        color: "transparent"

        Image {
            id: door1
            source: "qrc:/UI/assests/car_door.png"
            width: 32
            height: 32
            fillMode: Image.PreserveAspectFit
            anchors {
                top: parent.top
                right: parent.right
                topMargin: 130
                rightMargin: 30
            }
        }
        Image {
            id: belt1
            source: "qrc:/UI/assests/belt.png"
            width: 32
            height: 32
            fillMode: Image.PreserveAspectFit
            anchors {
                top: parent.top
                left: door1.right
                topMargin: 130
                leftMargin: 5
            }
        }
    }

    color: "#e3e3e3"
}
