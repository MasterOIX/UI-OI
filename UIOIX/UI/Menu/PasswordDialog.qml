import QtQuick 2.15
import QtQuick.Controls 2.15

Popup {
    id: passwordDialog
    modal: true
    focus: true
    width: 300
    height: 180
    closePolicy: Popup.CloseOnEscape

    property string ssid: ""
    property string password: ""

    signal accepted(string password)
    signal cancelled()

    background: Rectangle {
        color: "white"
        border.color: "gray"
        radius: 6
    }

    Column {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 12

        Text {
            text: "Enter password for: " + passwordDialog.ssid
            font.pixelSize: 16
            wrapMode: Text.Wrap
            color: "black"
        }

        TextField {
            id: passwordField
            placeholderText: "Password"
            echoMode: TextInput.Password
            onTextChanged: passwordDialog.password = text
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Row {
            spacing: 10
            anchors.horizontalCenter: parent.horizontalCenter

            Button {
                text: "Cancel"
                onClicked: {
                    passwordDialog.cancelled()
                    passwordDialog.close()
                }
            }

            Button {
                text: "OK"
                onClicked: {
                    passwordDialog.accepted(passwordField.text)
                    passwordDialog.close()
                }
            }
        }
    }
}
