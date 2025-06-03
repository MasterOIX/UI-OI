import QtQuick 2.12
import QtQuick.Controls 2.12
import QtPositioning 5.12
import QtLocation 5.12
import Controllers 1.0

Item {
    anchors.fill: parent
    
    Plugin {
	    id: mapPlugin
	    name: "mapboxgl"
	}
    
    Map {
        id: map
        anchors.fill: parent
        center: valhalla_controller.currentLocation ? valhalla_controller.currentLocation : QtPositioning.coordinate(44.2015, 26.1989)
        zoomLevel: 19
        plugin: mapPlugin
        bearing: valhalla_controller.bearing
        // add a perspective tilt
        tilt: 80

        Behavior on center{
            CoordinateAnimation{
                duration: 1000
                easing.type: Easing.Linear
            }
        }

        Behavior on bearing{
            PropertyAnimation{
                duration: 1000
                easing.type: Easing.Linear
            }
        }

        MapPolyline {
            id: drivenPolyline
            line.width: 5
            line.color: "#4488ccff"  // transparent blue
            path: valhalla_controller.polylinePoints.slice(0, valhalla_controller.shapeIndex+1)
        }

        MapPolyline {
            id: remainingPolyline
            line.width: 5
            line.color: "blue"
            path: valhalla_controller.polylinePoints.slice(valhalla_controller.shapeIndex)
        }

        /*MapQuickItem {
            id: centerIconDinamic
            visible: false
            anchorPoint.x: iconImage.width / 2
            anchorPoint.y: iconImage.height / 2
            coordinate: valhalla_controller.currentLocation ? valhalla_controller.currentLocation : QtPositioning.coordinate(44.2015, 26.1989)
            sourceItem: Image {
                id: iconImage
                source: "qrc:/UI/assests/navigation.png"
                width: 32
                height: 32
            }
        }*/

        Image {
            id: centerIconStatic
            source: "qrc:/UI/assests/navigation.png"  // Replace with your icon path
            anchors.centerIn: parent
            width: 32
            height: 32
        }

        MapQuickItem {
            id: locationIcon
            anchorPoint.x: locationIcon.width / 2
            anchorPoint.y: locationIcon.height / 2
            coordinate: valhalla_controller.destination ? valhalla_controller.destination : QtPositioning.coordinate(44.2015, 26.1989)
            sourceItem: Image {
                id: location
                source: "qrc:/UI/assests/pin.png"
                width: 32
                height: 32
            }
        }
    }

    NavigationSearchBox {
        id: navigationSearchBox
    }

    Indications {
        id: indications
    }

    Connections {
        target: valhalla_controller
        function onShapeIndexChanged() {
            console.log("🔄 Updating paths with shapeIndex:", valhalla_controller.shapeIndex)
            drivenPolyline.path = valhalla_controller.polylinePoints.slice(0, valhalla_controller.shapeIndex+1)
            remainingPolyline.path = valhalla_controller.polylinePoints.slice(valhalla_controller.shapeIndex)
        }
    }
}
