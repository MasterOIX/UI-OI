import QtQuick 2.12

Item {
    id: acLine
    anchors.fill: parent
    Rectangle {
        id: leftMiddleLine
        anchors {
            left: parent.left
            leftMargin: 135
            verticalCenter: parent.verticalCenter
        }
        width: 152
        height: 2
    }

    Rectangle {
        id: rightMiddleLine
        anchors {
            right: parent.right
            rightMargin: 135
            verticalCenter: parent.verticalCenter
        }
        width: 152
        height: 2
    }

    Rectangle {
        id: bottomMiddleLine
        anchors {
            top: parent.top
            topMargin: 80
            horizontalCenter: parent.horizontalCenter
        }
        width: 360
        height: 2
    }

    Item {
        id: obliqueRightLine
        anchors.fill: parent

        Rectangle {
            id: diagonalRightConnector
            color: "white"
            height: 2

            width: Math.sqrt(
                Math.pow(rightMiddleLine.x - (bottomMiddleLine.x + bottomMiddleLine.width), 2) +
                Math.pow(rightMiddleLine.y - (bottomMiddleLine.y), 2)
            ) + 1

            x: bottomMiddleLine.x + bottomMiddleLine.width - 1
            y: bottomMiddleLine.y

            transform: Rotation {
                origin.x: 0
                origin.y: 0
                angle: Math.atan2(
                    rightMiddleLine.y - bottomMiddleLine.y,
                    rightMiddleLine.x - (bottomMiddleLine.x + bottomMiddleLine.width)
                ) * 180 / Math.PI
            }
        }
    }

    Item {
        id: obliqueLeftLine
        anchors.fill: parent

        Rectangle {
            id: diagonalLeftConnector
            color: "white"
            height: 2

            width: Math.sqrt(
                Math.pow(bottomMiddleLine.x - (leftMiddleLine.x + leftMiddleLine.width), 2) +
                Math.pow(bottomMiddleLine.y - (leftMiddleLine.y), 2)
            ) + 1

            x: leftMiddleLine.x + leftMiddleLine.width
            y: leftMiddleLine.y

            transform: Rotation {
                origin.x: 0
                origin.y: 0
                angle: Math.atan2(
                    bottomMiddleLine.y - leftMiddleLine.y,
                    bottomMiddleLine.x - (leftMiddleLine.x + leftMiddleLine.width))
                    * 180 / Math.PI
            }
        }
    }
}
