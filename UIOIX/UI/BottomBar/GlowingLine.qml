import QtQuick 2.12
import QtGraphicalEffects 1.0

Item {
    anchors {
        top: parent.top
        topMargin: 5
        left: parent.left
        right: parent.right
    }
    Rectangle {
        id: glowLine
        height: 2
        anchors.verticalCenter: parent.verticalCenter
        width: parent.width
        color: "#B366FF"
        visible: true
    }

    Glow {
        anchors.fill: glowLine
        radius: 8             // smaller, tight glow
        spread: 0.6
        samples: 32
        color: "#B366FF"
        source: glowLine
    }

    Rectangle {
        id: directionalGlowTail
        anchors {
            top: glowLine.bottom
            left: parent.left
            right: parent.right
        }
        height: parent.parent.height/2
        visible: glowLine.visible
        opacity: 0.6

        gradient: Gradient {
            GradientStop { position: 0.0; color: "#B366FF" }
            GradientStop { position: 1.0; color: "#00000000" } // fade to transparent
        }
    }
}
