import QtQuick 2.12
import QtGraphicalEffects 1.0

Item {
    id: indications
    visible: valhalla_controller.navigating
    anchors {
        right: parent.right
        top: parent.top
        rightMargin: 45
        topMargin: 40
    }

    width: 240
    height: 120

    Rectangle {
        id: indicationsBox
        anchors.fill: parent
        radius: 10
        color: "white"

        Rectangle {
            id: eta
            anchors {
                bottom: parent.bottom
                left: parent.left
                leftMargin: 10
                bottomMargin: 10
                right: parent.right
                rightMargin: 10
            }
            height: 35
            color: "#c4e8ff"
            radius: 5

            Image {
                id: pin
                source: "qrc:/UI/assets/pin.png"
                anchors.centerIn: parent
                width: 20
                height: 20
                fillMode: Image.PreserveAspectFit
            }

            Rectangle {
                anchors {
                    left: parent.left
                    top: parent.top
                    bottom: parent.bottom
                }
                width: parent.width/2
                color: "transparent"

                Text {
                    id: etaText
                    anchors {
                        centerIn: parent
                    }
                    color: "black"
                    font.pixelSize: 14
                    font.bold: true
                    text: Math.round(valhalla_controller.etaTime / 60) + " min"
                }
            }

            Rectangle {
                anchors {
                    right: parent.right
                    top: parent.top
                    bottom: parent.bottom
                }
                width: parent.width/2
                color: "transparent"

                Text {
                    id: kmText
                    anchors {
                        centerIn: parent
                    }
                    color: "black"
                    font.pixelSize: 14
                    font.bold: true
                    text: Math.floor(valhalla_controller.remainingDistance * 100) / 100 + " km"
                }
            }
        }
        Rectangle {
            id: instructionsBox
            anchors {
                left: parent.left
                leftMargin: 10
                top: parent.top
                topMargin: 10
                right: parent.right
                rightMargin: 60
            }
            height: 60  // make it taller to allow for wrapping
            color: "white"

            Text {
                id: instructionsText
                width: parent.width
                anchors {
                    top: parent.top
                    left: parent.left
                }
                wrapMode: Text.WordWrap
                maximumLineCount: 4
                text: valhalla_controller.navigationUpdatedInstruction
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignTop
                elide: Text.ElideRight
                font.pixelSize: 12
            }
        }

        Rectangle {
            id: distanceToNextInstruction
            anchors {
                left: instructionsBox.right
                top: closeButton.bottom
                right: parent.right
                rightMargin: 10
                bottom: eta.top
            }
            color: "white"
            Text {
                id: distanceText
                anchors.centerIn: parent
                font.pixelSize: 12
                font.bold: true
                text: Math.floor(valhalla_controller.distanceToNextManeuver) + " m"
            }

        }


        Rectangle {
            id: closeButton
            anchors {
                top: parent.top
                right: parent.right
                rightMargin: 10
                topMargin: 10
            }
            width: 24
            height: 24
            color: "red"
            radius: 12
            Image {
                id: close
                source: "qrc:/UI/assets/close.png"
                anchors.centerIn: parent
                width: 10
                height: 10
                fillMode: Image.PreserveAspectFit
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    valhalla_controller.stopSimulation()
                }
            }
        }
    }

    DropShadow {
        anchors.fill: indicationsBox
        source: indicationsBox
        horizontalOffset: 2
        verticalOffset: 2
        radius: 8
        samples: 16
        color: "#44000000"
        z: -9999
    }
}
