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
            verticalCenter: parent.verticalCenter

        }
        height: parent.height * 0.65
        width: 120
        fillMode: Image.PreserveAspectFit
        source: "qrc:/UI/assests/car_front_icon.png"
    }

    VolumeControlComponent {
        id: volumeControl
    }

    HVACButton {
        id: syncController
        value: hvacHandler.syncEnabled
        toggleFunction: function(newState) {
            hvacHandler.syncEnabled = newState
            console.log("Sync enabled: " + newState)
        }
        buttonTxt: "Sync"
        anchors {
            top: parent.top
            bottom: parent.bottom
            left: carSettingsIcon.right
            leftMargin: 10
        }
    }

    HVACButton {
        id: acController
        value: hvacHandler.ACEnabled
        toggleFunction: function(newState) {
            hvacHandler.ACEnabled = newState
        }
        buttonTxt: "A/C"
        anchors {
            top: parent.top
            bottom: parent.bottom
            right: volumeControl.left
            rightMargin: 10
        }
    }


    HVACTemperature {
        id: driverHVACTemperature
        zoneModel: hvacHandler.driverZone

        anchors {
            left: syncController.right
            leftMargin: 20
        }
    }

    HVACTemperature {
        id: passengerHVACTemperature
        zoneModel: hvacHandler.passengerZone
        anchors {
            right: acController.left
            rightMargin: 20
        }
    }

    Connections {
        target: hvacHandler
        onSyncEnabledChanged: {
            syncController.value = hvacHandler.syncEnabled
        }
    }

    Connections {
        target: hvacHandler.driverZone
        onTemperatureChanged: {
            if (hvacHandler.syncEnabled) {
                hvacHandler.passengerZone.temperature = hvacHandler.driverZone.temperature
            }
        }
    }

    Connections {
        target: hvacHandler.passengerZone
        onTemperatureChanged: {
            if (hvacHandler.syncEnabled) {
                hvacHandler.syncEnabled = false
            }
        }
    }
}
