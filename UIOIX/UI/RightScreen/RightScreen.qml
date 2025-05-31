import QtQuick 2.12
import QtLocation 5.12
import QtPositioning 5.12
import QtGraphicalEffects 1.0

Rectangle {
    id: rightScreen

    anchors {
        top: parent.top
        bottom: bottomBar.top
        right: parent.right
    }
    width: parent.width * 9 / 15

    MapScreen {
        id: mapScreen
    }

    MediaPlayer {
        id: mediaPlayer
    }

}
