import QtQuick 2.12
import QtGraphicalEffects 1.0


Rectangle {
    id: bottomBar

    property double bar_height: parent.height / 11

    anchors {
        bottom: parent.bottom
        left: parent.left
        right: parent.right
    }
    height: parent.height / 11
    color: "black"

    GlowingLine {
        id: glowLine
    }

    GlowingTriangle {
        anchors.bottom: glowLine.top
        anchors.horizontalCenter: driverHVACTemperature.horizontalCenter
    }

    GlowingTriangle {
        anchors.bottom: glowLine.top
        anchors.horizontalCenter: passengerHVACTemperature.horizontalCenter
    }

    Image {
        id: carSettingsIcon
        anchors {
            left: parent.left
            top: parent.top
            topMargin: (bar_height * 0.35)/2 + 3
        }
        height: bar_height * 0.65
        width: 120
        fillMode: Image.PreserveAspectFit
        source: "qrc:/UI/assests/car_front_icon.png"
        visible: parent.height < bar_height * 1.25
    }

    VolumeControlComponent {
        id: volumeControl
        visible: parent.height < bar_height * 1.25
    }

    HVACButton {
        id: syncController
        value: hvacHandler.syncEnabled
        toggleFunction: function(newState) {
            hvacHandler.syncEnabled = newState
        }
        buttonTxt: "Sync"
        anchors {
            top: parent.top
            left: carSettingsIcon.right
            leftMargin: 10
        }
        height: bar_height
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
            right: volumeControl.left
            rightMargin: 10
        }
        height: bar_height
    }

    HVACButton {
        id: autoDriver
        value: hvacHandler.driverZone.autoEnabled
        toggleFunction: function(newState) {
            hvacHandler.driverZone.autoEnabled = newState
        }
        buttonTxt: "Auto"
        anchors {
            top: acController.bottom
            topMargin: - 5
            right: rearDefrost.left
        }
        height: bar_height
        visible: parent.height > bar_height * 1.75
    }

    HVACButton {
        id: autoPassenger
        value: hvacHandler.passengerZone.autoEnabled
        toggleFunction: function(newState) {
            hvacHandler.passengerZone.autoEnabled = newState
        }
        buttonTxt: "Auto"
        anchors {
            top: syncController.bottom
            topMargin: - 5
            left: recircAir.right
        }
        height: bar_height
        visible: parent.height > bar_height * 1.75
    }

    HVACButton {
        id: recircAir
        value: hvacHandler.airFromOutside
        toggleFunction: function(newState) {
            hvacHandler.airFromOutside = newState
        }
        anchors {
            top: passengerHVACTemperature.bottom
            topMargin: - 8
            left: passengerHVACTemperature.left
        }
        height: bar_height
        Image {
            id: air
            source: "qrc:/UI/assests/air.png"
            anchors.centerIn: parent
            fillMode: Image.PreserveAspectFit
            width: 30
            height: 29
        }
        visible: parent.height > bar_height * 1.75
    }

    HVACButton {
        id: rearDefrost
        value: hvacHandler.rearHeater
        toggleFunction: function(newState) {
            hvacHandler.rearHeater = newState
        }
        anchors {
            top: driverHVACTemperature.bottom
            topMargin: - 8
            right: driverHVACTemperature.right
        }
        height: bar_height
        Image {
            id: rear
            source: "qrc:/UI/assests/rear.png"
            anchors.centerIn: parent
            fillMode: Image.PreserveAspectFit
            width: 26
            height: 25
        }
        visible: parent.height > bar_height * 1.75
    }

    SeatHeaterIndicator {
        id: driverSeatHeat
        zone: hvacHandler.driverZone
        anchors {
            top: rearDefrost.top
            topMargin: 18
            horizontalCenter: syncController.horizontalCenter
        }
        visible: parent.height > bar_height * 1.75
    }

    SeatHeaterIndicator {
        id: passengerSeatHeat
        zone: hvacHandler.passengerZone
        anchors {
            top: recircAir.top
            topMargin: 18
            horizontalCenter: acController.horizontalCenter
        }
        visible: parent.height > bar_height * 1.75
    }


    HVACTemperature {
        id: driverHVACTemperature
        zoneModel: hvacHandler.driverZone

        anchors {
            top: parent.top
            topMargin: 3
            left: syncController.right
            leftMargin: 20
        }
    }

    HVACTemperature {
        id: passengerHVACTemperature
        zoneModel: hvacHandler.passengerZone
        anchors {
            top: parent.top
            topMargin: 3
            right: acController.left
            rightMargin: 20
        }
    }

    Connections {
        target: hvacHandler
        function onSyncEnabledChanged() {
            syncController.value = hvacHandler.syncEnabled
        }
    }

    Rectangle{
        anchors {
            top: parent.top
            topMargin: bar_height/2 - 1 + 4
        }
        color: "red"
        width: parent.width
        height: 2
        visible: false
    }

    CarSeat {
        id: driverSeat
        zoneModel: hvacHandler.driverZone
        seat_path: "qrc:/UI/assests/driver_seat.png"
        anchors {
            top: parent.top
            left: parent.left
            leftMargin: 0
            bottom: parent.bottom
        }
        width: 140
        visible: parent.height > bar_height * 1.75
    }

    CarSeat {
        id: passengerSeat
        zoneModel: hvacHandler.passengerZone
        seat_path: "qrc:/UI/assests/driver_seat.png"
        anchors {
            top: parent.top
            left: parent.left
            bottom: parent.bottom
        }
        width: 140

        transform: Scale {
            xScale: -1
            yScale: 1
            origin.x: width / 2
            origin.y: height / 2
        }
        visible: parent.height > bar_height * 1.75
    }

    ACLine {
        id: acLine
        visible: parent.height > bar_height * 1.75
    }

    Glow {
        anchors.fill: acLine
        radius: 32             // smaller, tight glow
        spread: 0.5
        samples: 64
        color: "#FDFDFD"
        source: acLine
        visible: parent.height > bar_height * 1.75
    }

    FanSpeed {
        id: fanSpeed
        visible: parent.height > bar_height * 1.75
    }

    Timer {
        id: collapseTimer
        interval: 4000
        repeat: false
        running: false
        onTriggered: {
            bottomBar.height = bar_height
        }
    }

    MouseArea {
        id: activityCatcher
        anchors.fill: parent
        hoverEnabled: true
        acceptedButtons: Qt.AllButtons
        propagateComposedEvents: true
        z: 9999
        onPressed: {
            if (bottomBar.height !== bar_height) {
                collapseTimer.stop()
                collapseTimer.start()
            }
            mouse.accepted = false
        }
        onReleased: {
            if (bottomBar.height !== bar_height) {
                collapseTimer.restart()
            }
        }
    }

    Behavior on height {
        NumberAnimation {
            duration: 300
            easing.type: "Linear"
        }
    }
}
