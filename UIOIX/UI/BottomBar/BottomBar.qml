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
        height: parent.height * 0.85
        fillMode: Image.PreserveAspectFit
        source: "qrc:/UI/assests/car_front_icon.png"
    }
}
