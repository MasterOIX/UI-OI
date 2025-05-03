import QtQuick 2.12
import QtGraphicalEffects 1.12
import QtQuick.Controls 2.12

Item {
    DropShadow {
        anchors.fill: navigationSearchBox
        source: navigationSearchBox
        horizontalOffset: 2
        verticalOffset: 2
        radius: 8           // Smaller blur radius
        samples: 16         // Fewer samples for a soft effect
        color: "#44000000"
        z: -9999
    }

    anchors {
        left: parent.left
        leftMargin: 45
        top: parent.top
        topMargin: 40
    }
    width: 250
    height: 40

    Rectangle {
        id: navigationSearchBox

        color: "#f7f7f7"
        radius: 2

        anchors.fill: parent

        Image {
            id: searchIcon
            source: "qrc:/UI/assests/search.png"
            anchors {
                verticalCenter: parent.verticalCenter
                left: parent.left
                leftMargin: 20
            }
            width: 20
            fillMode: Image.PreserveAspectFit
            smooth: true
        }

        Text {
            id: navigationPlaceholderText
            visible: navigationTextInput.text.length === 0
            anchors {
                verticalCenter: parent.verticalCenter
                left: searchIcon.right
                leftMargin: 10
                right: parent.right
                rightMargin: 20
            }
            color: "black"
            font.pixelSize: 14
            text: qsTr("Navigate")
        }
        TextInput {
            id: navigationTextInput
            anchors{
                left: navigationPlaceholderText.left
                right: navigationPlaceholderText.right
                bottom: navigationPlaceholderText.bottom
                top: navigationPlaceholderText.top
            }

            clip: true
            wrapMode: TextInput.NoWrap

            color: "black"
            font.pixelSize: 14
        }
    }

}
