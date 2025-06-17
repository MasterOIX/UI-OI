import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Rectangle {
    color: "transparent"

    property var selectedNetwork

    Rectangle {
        id: bluetoothBox
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }
        height: bluetoothController.bluetoothEnabled ? parent.height / 2 : 40

        color: "transparent"

        Text {
            id: bluetoothText
            text: qsTr("Bluetooth")
            anchors {
                top: parent.top
                left: parent.left
            }

            font.pixelSize: 20
            color: "white"
        }

        Text {
            visible: bluetoothController.bluetoothEnabled
            id: devicesText
            text: qsTr("Devices:")
            anchors {
                top: bluetoothText.bottom
                topMargin: 10
                left: parent.left
            }

            font.pixelSize: 20
            color: "white"
        }

        CustomSwitch {
            id: bluetoothToggle
            property bool btState: bluetoothController.bluetoothEnabled
            anchors {
                right: parent.right
                rightMargin: 10
                verticalCenter: bluetoothText.verticalCenter
            }
            checked: btState

            onToggled: {
                btState = checked
                console.log("Switch toggled to", checked)
                bluetoothController.setBluetoothEnabled(checked)
            }
        }

        Rectangle {
            id: newDeviceButton
            visible: bluetoothController.bluetoothEnabled
            anchors {
                horizontalCenter: bluetoothToggle.horizontalCenter
                verticalCenter: devicesText.verticalCenter
            }
            width: 90
            height: 25
            color: "#4CAF50"
            radius: 5
            Text {
                anchors.centerIn: parent
                text: qsTr("New Device")
                color: "white"
                font.bold: true
                font.pixelSize: 12
            }
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    console.log("Start pairing new Bluetooth device");
                    bluetoothController.startPairingAgent();
                }
            }
        }

        Rectangle {
            visible: bluetoothController.bluetoothEnabled
            id: devicesBox
            anchors {
                top: devicesText.bottom
                topMargin: 10
                left: parent.left
                right: parent.right
                bottom: parent.bottom
                bottomMargin: 20
            }
            color: "#e0e0e0"

            Rectangle {
                id: btHeader
                width: parent.width
                height: 30
                color: "#c0c0c0"
                anchors.top: parent.top

                Row {
                    anchors.fill: parent
                    anchors.margins: 6

                    Text {
                        text: "Name"
                        width: 180
                        font.bold: true
                        color: "black"
                        font.pixelSize: 16
                    }

                    Text {
                        text: "MAC"
                        width: 180
                        font.bold: true
                        color: "black"
                        font.pixelSize: 16
                    }

                    Text {
                        text: "Connection"
                        width: 100
                        font.bold: true
                        color: "black"
                        font.pixelSize: 16
                    }
                }
            }

            ListView {
                id: btList
                anchors {
                    top: btHeader.bottom
                    left: parent.left
                    right: parent.right
                    bottom: parent.bottom
                }
                model: bluetoothController.pairedDevices
                clip: true

                delegate: Rectangle {
                    width: btList.width
                    height: 30
                    color: index % 2 === 0 ? "#ffffff" : "#f0f0f0"

                    Row {
                        anchors.fill: parent
                        anchors.margins: 6

                        Text {
                            text: modelData.name
                            width: 180
                            color: "black"
                            verticalAlignment: Text.AlignVCenter
                            horizontalAlignment: Text.AlignLeft
                            font.pixelSize: 14
                        }

                        Text {
                            text: modelData.mac
                            width: 180
                            color: "black"
                            verticalAlignment: Text.AlignVCenter
                            horizontalAlignment: Text.AlignLeft
                            font.pixelSize: 14
                        }

                        Text {
                            width: 100
                            text: modelData.connected ? "Disconnect" : "Connect"
                            color: modelData.connected ? "green" : "black"
                            verticalAlignment: Text.AlignVCenter
                            horizontalAlignment: Text.AlignLeft
                            font.underline: true
                            font.bold: modelData.connected
                            font.pixelSize: 14

                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    console.log("Clicked on device:", modelData.name, modelData.mac);
                                    if (modelData.connected) {
                                        bluetoothController.disconnectFromDevice();
                                    } else {
                                        bluetoothController.connectToPairedDevice(modelData.mac);
                                    }
                                }
                            }
                        }
                    }
                }
                ScrollBar.vertical: ScrollBar {
                    policy: ScrollBar.AsNeeded
                    width: 6
                }
            }
        }
    }

    Rectangle {
        id: wirelessBox
        anchors {
            top: bluetoothBox.bottom
            left: parent.left
            right: parent.right
        }
        height: parent.height / 2

        color: "transparent"

        Text {
            id: wirelessText
            text: qsTr("WiFi")
            anchors {
                top: parent.top
                left: parent.left
            }

            font.pixelSize: 20
            color: "white"
        }

        Text {
            id: devicesTextW
            visible: wifiController.wifiEnabled
            text: qsTr("Devices:")
            anchors {
                top: wirelessText.bottom
                topMargin: 10
                left: parent.left
            }
            font.pixelSize: 20
            color: "white"
        }

        CustomSwitch {
            id: wirelessToggle
            anchors {
                right: parent.right
                rightMargin: 10
                verticalCenter: wirelessText.verticalCenter
            }
            checked: wifiController.wifiEnabled

            onToggled: {
                wifiController.wifiEnabled = checked
            }
        }

        Rectangle {
            id: scanButton
            visible: wifiController.wifiEnabled
            anchors {
                horizontalCenter: wirelessToggle.horizontalCenter
                verticalCenter: devicesTextW.verticalCenter
            }
            width: 50
            height: 25
            color: "#4CAF50"
            radius: 5
            Text {
                anchors.centerIn: parent
                text: qsTr("Scan")
                color: "white"
                font.bold: true
                font.pixelSize: 12
            }
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    console.log("Scan for WiFi networks");
                    wifiController.scanNetworks();
                }
            }
        }

        Rectangle {
            id: devicesBoxW
            visible: wifiController.wifiEnabled
            anchors {
                top: devicesTextW.bottom
                topMargin: 10
                left: parent.left
                right: parent.right
                bottom: parent.bottom
            }
            color: "#e0e0e0"

            // Header
            Rectangle {
                id: wifiHeader
                width: parent.width
                height: 30
                color: "#c0c0c0"
                anchors.top: parent.top

                Row {
                    anchors.fill: parent
                    anchors.margins: 6

                    Text {
                        Layout.fillWidth: true
                        Layout.preferredWidth: 180
                        text: "Name"
                        font.bold: true
                        width: 180
                        color: "black"
                        font.pixelSize: 16
                    }

                    Text {
                        Layout.fillWidth: true
                        Layout.preferredWidth: 60
                        text: "Signal"
                        font.bold: true
                        width: 60
                        color: "black"
                        font.pixelSize: 16
                    }

                    Text {
                        text: "Security"
                        font.bold: true
                        width: 140
                        color: "black"
                        font.pixelSize: 16
                    }
                    Text {
                        width: 70
                        text: "Connection"
                        font.bold: true
                        color: "black"
                        font.pixelSize: 16
                    }
                }
            }

            ListView {
                id: wifiList
                anchors {
                    top: wifiHeader.bottom
                    left: parent.left
                    right: parent.right
                    bottom: parent.bottom
                }
                model: wifiController.availableNetworks
                clip: true

                delegate: Rectangle {
                    width: wifiList.width
                    height: 30
                    color: index % 2 === 0 ? "#ffffff" : "#f0f0f0"

                    Row {
                        anchors.fill: parent
                        anchors.margins: 6

                        Text {
                            text: modelData.name
                            width: 180
                            color: "black"
                            font.bold: true
                            verticalAlignment: Text.AlignVCenter
                            horizontalAlignment: Text.AlignLeft
                            font.pixelSize: 14
                        }

                        Text {
                            text: modelData.signal
                            width: 60
                            color: "black"
                            verticalAlignment: Text.AlignVCenter
                            horizontalAlignment: Text.AlignLeft
                            font.pixelSize: 14
                        }

                        Text {
                            text: modelData.security
                            width: 140
                            color: "black"
                            verticalAlignment: Text.AlignVCenter
                            horizontalAlignment: Text.AlignLeft
                            font.pixelSize: 14
                        }

                        Text {
                            width: 70
                            text: wifiController.currentNetwork === modelData ? "Disconnect" : "Connect"
                            color: wifiController.currentNetwork === modelData ? "green" : "black"
                            verticalAlignment: Text.AlignVCenter
                            horizontalAlignment: Text.AlignLeft
                            font.underline: true
                            font.bold: wifiController.currentNetwork === modelData
                            font.pixelSize: 14

                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    selectedNetwork = modelData;

                                    if (wifiController.currentNetwork === modelData) {
                                        wifiController.disconnectFromCurrentNetwork();
                                        return;
                                    }

                                    if (modelData.security !== "" && !wifiController.isKnownAndTrusted(modelData.name)) {
                                        wifiPasswordPopup.ssid = modelData.name;
                                        wifiPasswordPopup.open();
                                    } else {
                                        wifiController.connectToNetwork(modelData);
                                    }
                                }
                            }
                        }
                    }
                }
                ScrollBar.vertical: ScrollBar {
                    policy: ScrollBar.AsNeeded
                    width: 6
                }
            }
        }
    }

    PasswordDialog {
        id: wifiPasswordPopup

        onAccepted: function(pwd) {
            if (selectedNetwork) {
                selectedNetwork.password = pwd;
                wifiController.connectToNetwork(selectedNetwork);
            }
        }

        onCancelled: {
            selectedNetwork = null
        }
    }

    Connections {
        target: wifiController

        function onAvailableNetworksChanged() {
            wifiList.model = wifiController.availableNetworks;
        }

        function onConnectionFailedChanged() {
            if (wifiController.connectionFailed && selectedNetwork) {
                if (selectedNetwork.security !== "") {
                    wifiPasswordPopup.ssid = selectedNetwork.name;
                    wifiPasswordPopup.open();
                }
            }
        }
    }
    Connections {
        target: bluetoothController

        function onPairedDevicesChanged() {
            btList.model = bluetoothController.pairedDevices;
        }
    }

    Dialog {
        id: pairingPromptDialog
        property string text: ""
        title: qsTr("Bluetooth Pairing Confirmation")
        modal: true
        standardButtons: Dialog.Ok | Dialog.Cancel

        // Make dialog size reasonable (optional)
        width: 350
        height: 180

        // Center on screen or parent window
        x: (parent ? parent.width : Screen.width) / 2 - width / 2
        y: (parent ? parent.height : Screen.height) / 2 - height / 2

        contentItem: Column {
            anchors.centerIn: parent
            spacing: 16
            Text {
                text: pairingPromptDialog.text
                wrapMode: Text.WordWrap
                horizontalAlignment: Text.AlignHCenter
            }
        }

        onAccepted: {
            bluetoothController.confirmPairing(true)
        }
        onRejected: {
            bluetoothController.confirmPairing(false)
        }
    }


    Connections {
        target: bluetoothController
        function onPairingPromptReceived(prompt) {
            console.log("Pairing prompt received:", prompt)
            pairingPromptDialog.text = prompt
            pairingPromptDialog.open()
        }
    }
}
