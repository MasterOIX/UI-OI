import QtQuick 2.15
import QtQuick.Controls 2.15
import QtGraphicalEffects 1.15

Rectangle {
    id: menuList
    anchors {
        top: parent.top
        bottom: parent.bottom
        left: parent.left
        right: subMenu.left
    }
    color: "#2d2d2d"

    ListModel {
        id: menuModel
        ListElement { text: "Connectivity" }
        ListElement { text: "Media" }
        ListElement { text: "Settings" }
        ListElement { text: "Trip Computer" }
        ListElement { text: "Vehicle Status" }
    }

    property int selectedIndex: 0
    property string selectedItem: menuModel[selectedIndex] ? menuModel[selectedIndex].text : "Connectivity"

    Column {
        id: menuColumn
        anchors.centerIn: parent
        spacing: 20

        Repeater {
            model: menuModel

            delegate: Rectangle {
                width: menuList.width
                height: 40
                color: "transparent"

                Rectangle {
                       anchors.fill: parent
                       color: "transparent"

                       visible: index === menuList.selectedIndex

                       LinearGradient {
                           anchors.fill: parent
                           start: Qt.point(0, 0)
                                       end: Qt.point(parent.width, 0)  // Left to right
                           gradient: Gradient {
                               GradientStop { position: 0.0; color: "#D91a4b8c" }  // 85% opacity blue
                               GradientStop { position: 1.0; color: "#001a4b8c" }  // 0% opacity blue
                           }
                       }
                   }

                Text {
                    text: model.text
                    anchors.centerIn: parent
                    color: "white"
                    font.pixelSize: 20
                    font.bold: index === menuList.selectedIndex
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        menuList.selectedIndex = index;
                        menuList.selectedItem = model.text;
                    }
                }
            }
        }
    }

    Item {
        anchors {
            right: parent.right
            top: parent.top
            bottom: parent.bottom
            topMargin: 40
            bottomMargin: 40
        }
        width: 1

        Rectangle {
            id: lineCore
            anchors.fill: parent
            color: "white"  // Same as top gradient (strong blue)
            radius: 10
        }

        Glow {
            anchors.fill: lineCore
            source: lineCore
            radius: 12          // Stronger glow
            color: "#D91a4b8c"
            samples: 16
            spread: 0.5
        }

        DropShadow {
            anchors.fill: lineCore
            source: lineCore
            radius: 10
            samples: 32
            color: "#801a4b8c"   // Deeper semi-transparent shadow
            horizontalOffset: 0.5
            verticalOffset: 0
        }
    }
}
