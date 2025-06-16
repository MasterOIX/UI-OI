import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtQuick.Dialogs 1.0
import QtQuick.Extras 1.4

Rectangle {
    color: "transparent"

    property color selectedColor: systemHandler.interiorColor
    property color tempColor: selectedColor

    Text {
        id: interiorColor
        text: qsTr("Interior Color")
        anchors {
            top: parent.top
            left: parent.left
            topMargin: 20
            leftMargin: 20
        }
        font.pixelSize: 20
        color: "white"
    }

    Rectangle {
        id: colorSelector
        width: 100
        height: interiorColor.height
        anchors {
            right: parent.right
            verticalCenter: interiorColor.verticalCenter
            rightMargin: 20
        }
        color: selectedColor
        border.color: "white"
        border.width: 1

        MouseArea {
            anchors.fill: parent
            onClicked: {
                tempColor = selectedColor
                colorDialog.open()
            }
        }
    }

    // Measurement Units
    Text {
        id: measurementUnit
        text: qsTr("Measurement Units")
        anchors {
            top: interiorColor.bottom
            left: parent.left
            topMargin: 30
            leftMargin: 20
        }
        font.pixelSize: 20
        color: "white"
    }

    Rectangle {
        id: temperatureUnitSelector
        width: 100
        height: measurementUnit.height
        anchors {
            right: parent.right
            verticalCenter: measurementUnit.verticalCenter
            rightMargin: 20
        }
        color: "lightgray"
        border.color: "white"
        border.width: 1
        Text {
            id: tempUnit
            text: systemHandler.tempUnit
            anchors.centerIn: parent
            color: "black"
            font.pixelSize: 12
            font.bold: true
        }
        MouseArea {
            anchors.fill: parent
            onClicked: {
                tempUnit.text = (tempUnit.text === "C°") ? "F°" : "C°";
                systemHandler.tempUnit = tempUnit.text;
            }
        }
    }

    Rectangle {
        id: speedUnitSelector
        width: 100
        height: measurementUnit.height
        anchors {
            top: temperatureUnitSelector.bottom
            topMargin: 10
            horizontalCenter: temperatureUnitSelector.horizontalCenter
        }
        color: "lightgray"
        border.color: "white"
        border.width: 1
        Text {
            id: speedUnit
            text: systemHandler.speedUnit
            anchors.centerIn: parent
            color: "black"
            font.pixelSize: 12
            font.bold: true
        }
        MouseArea {
            anchors.fill: parent
            onClicked: {
                speedUnit.text = (speedUnit.text === "km/h") ? "mph" : "km/h";
                systemHandler.speedUnit = speedUnit.text;
            }
        }
    }

    Text {
        id: speedWarning
        text: qsTr("Speed warning at:")
        anchors {
            top: speedUnitSelector.bottom
            left: parent.left
            topMargin: 30
            leftMargin: 20
        }
        font.pixelSize: 20
        color: "white"
    }

    Rectangle {
        id: speedWarningSelector
        width: 100
        height: speedWarning.height
        anchors {
            right: parent.right
            verticalCenter: speedWarning.verticalCenter
            rightMargin: 20
        }
        color: "lightgray"
        border.color: "white"
        border.width: 1
        Text {
            id: speedWarningValue
            text: (systemHandler.speedUnit === "mph"
                         ? systemHandler.convertKMHToMPH(systemHandler.speedWarning)
                         : systemHandler.speedWarning) + systemHandler.speedUnit
            anchors.centerIn: parent
            color: "black"
            font.pixelSize: 12
            font.bold: true
        }
        MouseArea {
            anchors.fill: parent
            onClicked: {
                speedDialPopup.open()
            }
        }
    }


    ColorDialog {
        id: colorDialog
        color: selectedColor  // Default color
        title: "Please choose a color"
        currentColor: selectedColor
        showAlphaChannel: true

        onAccepted: {
            onAccepted: {
                systemHandler.interiorColor = colorDialog.color
            }
        }
    }

    Popup {
        id: speedDialPopup
        width: 260
        height: 320
        modal: true
        focus: true
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

        // Convertire dinamică în funcție de unitate
        property int maxSpeed: systemHandler.speedUnit === "mph" ? systemHandler.convertKMHToMPH(260) : 260
        property int initialSpeed: systemHandler.speedUnit === "mph"
                                    ? systemHandler.convertKMHToMPH(systemHandler.speedWarning)
                                    : systemHandler.speedWarning
        property int step: 5

        background: Rectangle {
            color: "#2d2d2d"
            radius: 12
            border.color: "white"
        }

        Column {
            anchors.centerIn: parent
            spacing: 10

            Dial {
                id: speedDial
                maximumValue: speedDialPopup.maxSpeed
                stepSize: speedDialPopup.step
                value: speedDialPopup.initialSpeed
                tickmarksVisible: true
                width: 200
                height: 200

                onValueChanged: {
                    speedValueLabel.text = "Speed Warning: " + speedDial.value + systemHandler.speedUnit;
                }
            }

            Text {
                id: speedValueLabel
                text: "Speed Warning: " + speedDialPopup.initialSpeed + systemHandler.speedUnit
                color: "white"
                font.pixelSize: 18
                horizontalAlignment: Text.AlignHCenter
                width: parent.width
            }

            Row {
                spacing: 20
                anchors.horizontalCenter: parent.horizontalCenter

                Button {
                    text: "Cancel"
                    onClicked: speedDialPopup.close()
                }

                Button {
                    text: "Apply"
                    onClicked: {
                        // Convertim înapoi în km/h dacă suntem pe mph
                        let valueInKmh = systemHandler.speedUnit === "mph"
                            ? systemHandler.convertKMHToMPH(speedDial.value)
                            : speedDial.value

                        systemHandler.speedWarning = valueInKmh

                        // Actualizare text selector din pagină principală
                        speedWarningSelector.children[0].text = speedDial.value + systemHandler.speedUnit

                        speedDialPopup.close()
                    }
                }
            }
        }
    }
}
