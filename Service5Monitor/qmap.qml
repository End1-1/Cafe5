import QtQuick 2.5
import QtQuick.Window 2.3
import QtPositioning 5.12
import QtLocation 5.12



Item {
    id: wnd
    height: 500
    width: 500

    Plugin {
        id: mapPlugin
        name: "osm"
    }

    Map {
        id: map
        anchors.fill: parent
        plugin: mapPlugin
        center: QtPositioning.coordinate( 40.1947, 44.475)
        zoomLevel: 14



        MapItemView {
            model: ListModel {
                id: markersModel
            }
            delegate: Component {
                MapQuickItem {
                    id: point
                    property var user: 0
                       coordinate: QtPositioning.coordinate(latitude, longitude)
                       anchorPoint.x: image.width/2
                       anchorPoint.y: image.height/2
                       rotation: azimuth
                       sourceItem: Column {
                           Image {
                               id: image;
                               source: "car_up.png";
                               height: 30
                               width: 30
                           }

                           Text {
                               //text: "title";
                               font.bold: true
                           }
                       }
                   }

            }
        }

    }

    Text {
        id: objectCount
        text: "0"
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.margins: 20
    }

    function clearMarkers() {
        markersModel.clear()
    }

    function createMarker(lat, lon, azimuth, user) {
        markersModel.append({ "latitude": lat, "longitude": lon, "azimuth": azimuth, "user": user})
        objectCount.text = markersModel.count
    }

    function updateMarker(lat,lon, azimuth, user) {
//        var index = userindex[user]
//        markersModel.remove(index)
//        createMarker(lat,lon,azimuth,user)

        for (var i = 0; i < markersModel.count; i++) {
            var pp = markersModel.get(i)
            if (pp.user === user) {
                pp.coordinate = QtPositioning.coordinate(lat, lon)
                markersModel.remove(i)
                createMarker(lat,lon,azimuth,user)
                return
            }
        }
    }

}
