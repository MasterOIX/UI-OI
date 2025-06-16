import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtQuick.Dialogs 1.3


Rectangle {
    id: root
    color: "transparent"
    property string currentFolder: ""

    ListView {
        id: songListView
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }
        height: 370

        spacing: 0
        clip: true
        model: audioController.localSongs

        delegate: Column {
            width: root.width

            Rectangle {
                width: root.width
                height: 40
                color: "transparent"

                Text {
                    text: modelData === ".." ? "\u23CE ../" : modelData.split("/").pop()
                    font.pixelSize: 16
                    color: "white"
                    elide: Text.ElideRight
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left
                    anchors.leftMargin: 12
                    width: parent.width - 24
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        if (modelData.endsWith(".mp3") || modelData.endsWith(".wav") || modelData.endsWith(".flac") || modelData.endsWith(".ogg") || modelData.endsWith(".aac")) {
                            audioController.playFromLocal(index)
                        } else {
                            if (modelData === "..") {
                                audioController.scanLocalMusic(currentFolder.split("/").slice(0, -1).join("/"))  // Go up one folder
                            } else {
                                currentFolder = modelData
                                audioController.scanLocalMusic(modelData)  // Go into folder
                            }
                        }
                    }
                }
            }

            Rectangle {
                width: parent.width
                height: 1
                color: "white"
            }
        }

        ScrollBar.vertical: ScrollBar {
            width: 8
            policy: ScrollBar.AsNeeded
        }
    }

    Button {
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
            leftMargin: 10
            rightMargin: 10
            top: songListView.bottom
            topMargin: 10
        }

        text: "Import from USB"
        font.pixelSize: 14
        onClicked: {
            var usbPath = audioController.usbPath

            if (usbPath === "") {
                warningDialog.open()
            } else {
                confirmDialog.text = "Do you want to import all music from:\n" + usbPath + "?"
                confirmDialog.open()
            }
        }
    }

    MessageDialog {
        id: warningDialog
        title: "No USB Detected"
        text: "No USB path found. Please insert a USB drive."
        icon: StandardIcon.Warning
        standardButtons: StandardButton.Ok
    }

    MessageDialog {
        id: confirmDialog
        title: "Import Music"
        icon: StandardIcon.Question
        text: "Do you want to import music from USB?"
        standardButtons: StandardButton.Yes | StandardButton.No
        onYes: audioController.importFromUsb()
    }

}
