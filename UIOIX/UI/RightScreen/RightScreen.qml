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

    Plugin {
        id: mapPlugin
        name: "mapboxgl"
    }

    Map {
        anchors {
            top: parent.top
            bottom: parent.bottom
            left: parent.left
            right: parent.right
            bottomMargin: -20
        }

        plugin: mapPlugin
        center: QtPositioning.coordinate(44.20147691467647, 26.19891163624485) // Colibasi
        zoomLevel: 13
    }

    NavigationSearchBox {
        id: navigationSearchBox
    }

    MediaPlayer {
        id: mediaPlayer
    }

}
