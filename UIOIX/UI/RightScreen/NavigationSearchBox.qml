import QtQuick 2.12
import QtGraphicalEffects 1.12
import QtQuick.Controls 2.12
import QtLocation 5.12
import QtPositioning 5.12

Item {
    id: root
    width: 250
    height: 200

    property string delayedQuery: ""
    Timer {
        id: queryDelayTimer
        interval: 400
        running: false
        repeat: false
        onTriggered: {
            delayedQuery = navigationTextInput.text
        }
    }

    Plugin {
        id: geocodePlugin
        name: "osm"
    }

    GeocodeModel {
        id: geocodeModel
        plugin: geocodePlugin
        query: delayedQuery
        limit: 5
        autoUpdate: true
    }

    DropShadow {
        anchors.fill: navigationSearchBox
        source: navigationSearchBox
        horizontalOffset: 2
        verticalOffset: 2
        radius: 8
        samples: 16
        color: "#44000000"
        z: -9999
    }

    anchors {
        left: parent.left
        leftMargin: 45
        top: parent.top
        topMargin: 40
    }

    Rectangle {
        id: navigationSearchBox
        property string selectedLocation: ""
        color: "#f7f7f7"
        radius: 2
        width: parent.width
        height: 40

        Image {
            id: searchIcon
            source: "qrc:/UI/assets/search.png"
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

        MouseArea {
            anchors.fill: parent
            onClicked: {
                if (valhalla_controller.navigating) {
                    navigationTextInput.focus = true
                    navigationTextInput.cursorVisible = true
                    navigationTextInput.cursorPosition = navigationTextInput.text.length
                    resultList.visible = true
                    resetTimer.stop()
                    resetTimer.start()
                }
            }
            propagateComposedEvents: true
        }

        TextInput {
            id: navigationTextInput
            anchors {
                left: navigationPlaceholderText.left
                right: navigationPlaceholderText.right
                verticalCenter: parent.verticalCenter
            }
            clip: true
            wrapMode: TextInput.NoWrap
            color: "black"
            font.pixelSize: 14
            cursorVisible: !valhalla_controller.navigating

            onTextChanged: {
                queryDelayTimer.restart()
                if (valhalla_controller.navigating) {
                    resetTimer.stop()
                    resetTimer.start()
                }
            }
            onActiveFocusChanged: {
                if (activeFocus) {
                    if (valhalla_controller.navigating) {
                        resetTimer.stop()
                        resetTimer.start()
                    }
                }
            }

            Timer {
                id: resetTimer
                interval: 12000 // 12 seconds
                running: false
                repeat: false
                onTriggered: {
                    if (valhalla_controller.navigating) {
                        console.log("🔄 Resetting navigation input")
                        navigationTextInput.cursorVisible = false
                        navigationTextInput.cursorPosition = 0
                        resultList.visible = false
                        navigationTextInput.text = navigationSearchBox.selectedLocation
                    }
                }
            }
        }
    }

    ListView {
        id: resultList
        width: navigationSearchBox.width
        height: 140
        y: navigationSearchBox.height + 5
        visible: geocodeModel.count > 0
        model: geocodeModel
        interactive: true
        clip: true
        spacing: 4

        delegate: Rectangle {
            id: itemRect
            width: 250
            height: 38
            radius: 6
            color: mouseArea.pressed ? "#d0eaff" : mouseArea.containsMouse ? "#f0f0f0" : "#ffffff"
            border.color: "#cccccc"
            border.width: 1

            property var location: (index >= 0 && index < geocodeModel.count) ? geocodeModel.get(index) : null

            Text {
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                anchors.leftMargin: 12
                text: location ? location.address.text : "Searching..."
                color: "#202020"
                font.pixelSize: 13
                elide: Text.ElideRight
                width: parent.width - 24
            }

            MouseArea {
                id: mouseArea
                anchors.fill: parent
                hoverEnabled: true
                cursorShape: Qt.PointingHandCursor
                onClicked: {
                    if (location) {
                        navigationSearchBox.selectedLocation = location.address.text
                        console.log("📍 Selected:", location.address.text, "at", location.coordinate.latitude, location.coordinate.longitude)
                        valhalla_controller.destination = location.coordinate
                        navigationTextInput.text = location.address.text
                        navigationTextInput.cursorPosition = 0
                        resultList.visible = false
                        valhalla_controller.startNavigation()
                    }
                }
            }
        }

        ScrollBar.vertical: ScrollBar {
            width: 6
            policy: ScrollBar.AsNeeded
        }
    }
}
