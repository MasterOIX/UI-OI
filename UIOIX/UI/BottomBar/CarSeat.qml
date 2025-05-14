import QtQuick 2.12
import QtGraphicalEffects 1.0
import HVAC 1.0

Item {
    property bool headZoneActive: zoneModel.isZoneActive(ZoneController.Head)
    property bool bodyZoneActive: zoneModel.isZoneActive(ZoneController.Body)
    property bool legZoneActive: zoneModel.isZoneActive(ZoneController.Legs)
    property alias seat_path: seat.source
    property var zoneModel

    Image {
        id: heat
        anchors {
            top: parent.top
            left: parent.left
            topMargin: 56
            leftMargin: 59
        }
        width: 25
        height: 23
        source: "qrc:/UI/assests/heater.png"
        fillMode: Image.PreserveAspectFit
        visible: zoneModel.seatHeating > 0
    }

    Image {
        id: seat
        anchors {
            centerIn: parent
            margins: 10
        }

        height: parent.height * 0.6
        width: 100
        fillMode: Image.PreserveAspectFit
    }

    Image {
        id: topAir
        anchors {
            top: parent.top
            left: parent.left
            topMargin: 25
            leftMargin: 59
        }
        width: 18
        height: 12
        source: "qrc:/UI/assests/arrow.png"
        fillMode: Image.PreserveAspectFit
        MouseArea {
            anchors {
                top: parent.top
                left: parent.left
                bottom: parent.bottom
                right: parent.right
                margins: -5
            }
            onClicked: {
                zoneModel.toggleZone(ZoneController.Head)
            }
        }
    }

    Image {
        id: middleAir
        anchors {
            top: parent.top
            left: parent.left
            topMargin: 45
            leftMargin: 77
        }
        width: 18
        height: 12
        source: "qrc:/UI/assests/arrow.png"
        fillMode: Image.PreserveAspectFit
        MouseArea {
            anchors {
                top: parent.top
                left: parent.left
                bottom: parent.bottom
                right: parent.right
                margins: -5
            }
            onClicked: {
                zoneModel.toggleZone(ZoneController.Body)
            }
        }
    }

    Image {
        id: legAir
        anchors {
            top: parent.top
            left: parent.left
            topMargin: 65
            leftMargin: 95
        }
        width: 18
        height: 12
        source: "qrc:/UI/assests/arrow_leg.png"
        fillMode: Image.PreserveAspectFit
        MouseArea {
            anchors {
                top: parent.top
                left: parent.left
                bottom: parent.bottom
                right: parent.right
                margins: -5
            }

            onClicked: {
                zoneModel.toggleZone(ZoneController.Legs)
            }
        }
    }

    Glow {
        id: legGlow
        anchors.fill: legAir
        source: legAir
        color: "#FF0000"
        radius: 10
        spread: 0.4
        samples: 16
        visible: legZoneActive
    }

    Glow {
        id: middleGlow
        anchors.fill: middleAir
        source: middleAir
        color: "#FF0000"
        radius: 10
        spread: 0.4
        samples: 16
        visible: bodyZoneActive
    }

    Glow {
        id: headGlow
        anchors.fill: topAir
        source: topAir
        color: "#FF0000"
        radius: 10
        spread: 0.4
        samples: 16
        visible: headZoneActive
    }

    Glow {
        visible: zoneModel.seatHeating > 0
        anchors.fill: heat
        source: heat
        color: "#FF0000"
        radius: 10
        spread: zoneModel.seatHeating === 1 ? 0.1
                  : zoneModel.seatHeating === 2 ? 0.3
                  : zoneModel.seatHeating === 3 ? 0.5
                  : 0.0
        samples: 16
    }

    Connections {
        target: zoneModel

        function onAirZoneMaskChanged() {
            headZoneActive = zoneModel.isZoneActive(ZoneController.Head)
            bodyZoneActive = zoneModel.isZoneActive(ZoneController.Body)
            legZoneActive = zoneModel.isZoneActive(ZoneController.Legs)
        }
    }
}
