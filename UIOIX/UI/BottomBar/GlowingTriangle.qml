import QtQuick 2.12
import QtGraphicalEffects 1.0

Item {
    id: glowingTriangle
    width: 20
    height: 3

    Canvas {
        id: triangleCanvas
        anchors.fill: parent
        onPaint: {
            var ctx = getContext("2d")
            ctx.clearRect(0, 0, width, height)
            ctx.beginPath()
            ctx.moveTo(0, height)
            ctx.lineTo(width / 2, 0)
            ctx.lineTo(width, height)
            ctx.closePath()
            ctx.fillStyle = "#B366FF"
            ctx.fill()
        }
    }

    Glow {
        anchors.fill: parent
        radius: 10
        spread: 0.6
        samples: 32
        color: "#B366FF"
        source: triangleCanvas
    }
}
