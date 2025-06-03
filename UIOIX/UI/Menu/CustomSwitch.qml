// BluetoothSwitch.qml
import QtQuick 2.15
import QtQuick.Controls 2.15

Item {
    id: root
    property alias checked: bluetoothSwitch.checked
    signal toggled(bool checked)

    width: 60
    height: 32

    Switch {
        id: bluetoothSwitch
        anchors.fill: parent
        checked: false
        onCheckedChanged: root.toggled(checked)

        indicator: Rectangle {
            id: track
            anchors.centerIn: parent
            width: 48
            height: 20
            radius: height / 2
            color: bluetoothSwitch.checked ? "#4CAF50" : "#aaa"

            Rectangle {
                id: thumb
                width: 28
                height: 28
                radius: 14
                y: (track.height - height) / 2  // slight vertical offset
                x: bluetoothSwitch.checked ? track.width - width : 0
                color: bluetoothSwitch.down ? "#cccccc" : "white"
                border.color: "#888"
                border.width: 1

                Behavior on x {
                    NumberAnimation {
                        duration: 150
                        easing.type: Easing.InOutQuad
                    }
                }
            }
        }
    }
}
