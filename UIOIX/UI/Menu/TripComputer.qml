import QtQuick 2.15
import QtQuick.Controls 2.15

Rectangle {
    id: root
    color: "transparent"
    readonly property string distanceUnit: systemHandler.distanceUnit
    readonly property string energyUnit: systemHandler.speedUnit

    Text {
        id: totalDistanceText
        text: "Total distance"
        anchors {
            top: parent.top
            topMargin: 10
            left: parent.left
        }
        font.pixelSize: 16
        color: "white"
    }

    Text {
        id: totalDistanceValue
        text: systemHandler.convertKilometersToMiles(infoController.totalDistance) + " " + distanceUnit
        anchors {
            top: totalDistanceText.top
            horizontalCenter: parent.horizontalCenter
        }
        font.pixelSize: 16
        color: "white"
        font.bold: true
    }

    Rectangle {
        width: parent.width
        height: 1
        color: "#ffffffef"
        anchors {
            top: totalDistanceText.bottom
            left: parent.left
            topMargin: 1
            right: totalDistanceValue.right
        }
    }

    Text {
        id: rangeText
        text: qsTr("Range")
        anchors {
            top: totalDistanceText.bottom
            left: parent.left
            topMargin: 20
        }
        font.pixelSize: 16
        color: "white"
    }

    Text {
        id: rangeValue
        text: systemHandler.convertKilometersToMiles(infoController.range) + " " + distanceUnit
        anchors {
            top: rangeText.top
            right: totalDistanceValue.right
        }
        font.pixelSize: 16
        color: "white"
        font.bold: true
    }

    Rectangle {
        width: parent.width
        height: 1
        color: "#ffffffef"
        anchors {
            top: rangeText.bottom
            left: parent.left
            topMargin: 1
            right: rangeValue.right
        }
    }

    Text {
        id: consumptionText
        text: qsTr("Consumption")
        anchors {
            top: rangeText.bottom
            left: parent.left
            topMargin: 20
        }
        font.pixelSize: 16
        color: "white"
    }

    Text {
        id: consumptionValue
        text: systemHandler.speedUnit === "mph"
              ? (100 * 33.7 / infoController.consumption).toFixed(1) + " MPGe"
              : infoController.consumption.toFixed(1) + " kWh/100km"
        anchors {
            top: consumptionText.top
            right: rangeValue.right
        }
        font.pixelSize: 16
        color: "white"
        font.bold: true
    }

    Rectangle {
        width: parent.width
        height: 1
        color: "#ffffffef"
        anchors {
            top: consumptionText.bottom
            left: parent.left
            topMargin: 1
            right: consumptionValue.right
        }
    }

    Text {
        id: averageSpeedText
        text: qsTr("Average Speed")
        anchors {
            top: consumptionText.bottom
            left: parent.left
            topMargin: 20
        }
        font.pixelSize: 16
        color: "white"
    }
    Text {
        id: averageSpeedValue
        text: systemHandler.convertKMHToMPH(infoController.averageSpeed) + " " + systemHandler.speedUnit
        anchors {
            top: averageSpeedText.top
            right: consumptionValue.right
        }
        font.pixelSize: 16
        color: "white"
        font.bold: true
    }
    Rectangle {
        width: parent.width
        height: 2
        color: "#ffffffef"
        anchors {
            top: averageSpeedText.bottom
            left: parent.left
            topMargin: 1
            right: parent.right
        }
    }

    Text {
        id: totalText
        text: qsTr("Total")
        anchors {
            verticalCenter: averageSpeedText.verticalCenter
            right: parent.right
        }
        font.pixelSize: 20
        color: "white"
        font.bold: true
    }

    Text {
        id: startTimeText
        text: qsTr("Start Time")
        anchors {
            top: totalText.bottom
            left: parent.left
            topMargin: 30
        }
        font.pixelSize: 16
        color: "white"
    }
    Text {
        id: startTimeValue
        text: infoController.startTime
        anchors {
            top: startTimeText.top
            right: totalDistanceValue.right
        }
        font.pixelSize: 16
        color: "white"
        font.bold: true
    }
    Rectangle {
        width: parent.width
        height: 1
        color: "#ffffffef"
        anchors {
            top: startTimeText.bottom
            left: parent.left
            topMargin: 1
            right: startTimeValue.right
        }
    }

    Text {
        id: tripDistanceText
        text: qsTr("Trip Distance")
        anchors {
            top: startTimeText.bottom
            left: parent.left
            topMargin: 20
        }
        font.pixelSize: 16
        color: "white"
    }

    Text {
        id: tripDistanceValue
        text: systemHandler.convertKilometersToMiles(infoController.tripDistance) + " " + distanceUnit
        anchors {
            top: tripDistanceText.top
            right: startTimeValue.right
        }
        font.pixelSize: 16
        color: "white"
        font.bold: true
    }

    Rectangle {
        width: parent.width
        height: 1
        color: "#ffffffef"
        anchors {
            top: tripDistanceText.bottom
            left: parent.left
            topMargin: 1
            right: tripDistanceValue.right
        }
    }

    Text {
        id: tripDurationText
        text: qsTr("Trip Duration")
        anchors {
            top: tripDistanceText.bottom
            left: parent.left
            topMargin: 20
        }
        font.pixelSize: 16
        color: "white"
    }

    Text {
        id: tripDurationValue
        text: infoController.tripDuration
        anchors {
            top: tripDurationText.top
            right: tripDistanceValue.right
        }
        font.pixelSize: 16
        color: "white"
        font.bold: true
    }

    Rectangle {
        width: parent.width
        height: 1
        color: "#ffffffef"
        anchors {
            top: tripDurationText.bottom
            left: parent.left
            topMargin: 1
            right: tripDurationValue.right
        }
    }

    Text {
        id: tripConsumptionText
        text: qsTr("Trip Consumption")
        anchors {
            top: tripDurationText.bottom
            left: parent.left
            topMargin: 20
        }
        font.pixelSize: 16
        color: "white"
    }

    Text {
        id: tripConsumptionValue
        text: systemHandler.speedUnit === "mph"
              ? (infoController.tripConsumption * 0.621371).toFixed(1) + " mi/kWh"
              : infoController.tripConsumption.toFixed(1) + " kWh"
        anchors {
            top: tripConsumptionText.top
            right: tripDurationValue.right
        }
        font.pixelSize: 16
        color: "white"
        font.bold: true
    }

    Rectangle {
        width: parent.width
        height: 1
        color: "#ffffffef"
        anchors {
            top: tripConsumptionText.bottom
            left: parent.left
            topMargin: 1
            right: tripConsumptionValue.right
        }
    }

    Text {
        id: tripAverageSpeedText
        text: qsTr("Trip Average Speed")
        anchors {
            top: tripConsumptionText.bottom
            left: parent.left
            topMargin: 20
        }
        font.pixelSize: 16
        color: "white"
    }

    Text {
        id: tripAverageSpeedValue
        text: systemHandler.convertKMHToMPH(infoController.tripAverageSpeed) + " " + systemHandler.speedUnit
        anchors {
            top: tripAverageSpeedText.top
            right: tripConsumptionValue.right
        }
        font.pixelSize: 16
        color: "white"
        font.bold: true
    }

    Text {
        id: trip
        text: qsTr("Trip")
        anchors {
            right: parent.right
            verticalCenter: tripAverageSpeedText.verticalCenter
        }
        font.pixelSize: 20
        color: "white"
        font.bold: true
    }

    Rectangle {
        width: parent.width
        height: 2
        color: "#ffffffef"
        anchors {
            top: tripAverageSpeedText.bottom
            left: parent.left
            topMargin: 1
            right: parent.right
        }
    }

    Text {
        id: resetAutomaticTripText
        text: qsTr("Reset Automatically")
        anchors {
            top: tripAverageSpeedText.bottom
            left: parent.left
            topMargin: 30
        }
        font.pixelSize: 16
        color: "white"
    }

    CheckBox {
        id: resetAutomaticTripCheckbox

        indicator.width: 20
        indicator.height: 20

        anchors {
            right: consumptionValue.right
            verticalCenter: resetAutomaticTripText.verticalCenter
        }

        checked: infoController.resetAutomatically
        onCheckedChanged: infoController.resetAutomatically = checked
    }

    Button {
        text: qsTr("Reset")
        anchors {
            right: parent.right
            verticalCenter: resetAutomaticTripText.verticalCenter
        }
        onClicked: infoController.resetTrip()
        width: 80
        height: 20
        background: Rectangle {
            color: "white"  // 85% opacity blue
            radius: 5
        }
    }
}
