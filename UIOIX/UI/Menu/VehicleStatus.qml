import QtQuick 2.12

Rectangle {
    color: "transparent"

    // Mock warning data: name + code (00 = minor, 01 = major)
    property var warnings: [
        { name: "Low Tire Pressure", code: "00" },
        { name: "Engine Overheat", code: "01" },
        { name: "Brake Fluid Low", code: "00" },
        { name: "Battery Fault", code: "01" }
    ]

    ListView {
        id: warningList
        anchors {
            left: parent.left
            leftMargin: 20
            top: parent.top
            bottom: parent.bottom
        }
        width: parent.width / 2
        model: warnings
        delegate: Item {
            width: warningList.width
            height: 30

            Image {
                id: warningIcon
                width: 30
                height: 30
                anchors.verticalCenter: parent.verticalCenter
                source: modelData.code === "01"
                        ? "qrc:/UI/assets/u_warning.png"
                        : "qrc:/UI/assets/warning.png"
            }

            Text {
                text: modelData.name
                color: "white"
                font.pixelSize: 16
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: warningIcon.right
                anchors.leftMargin: 20
                font.bold: true
            }
        }
    }
}
