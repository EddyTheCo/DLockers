import QtQuick
import QtQuick.Controls
import Esterv.CustomControls.CoordToMap
import QtPositioning

ApplicationWindow {
    visible: true
    id:window

    ObjectMapView
    {
        id: mapview
        anchors.fill:parent
        objModel: ListModel
        {
            ListElement {
                latitude: 41.902916
                longitude: 12.453389
                account: "rms1qddr45ddfdd333344ffttddsskfkkt"
                score:"4.5/5.0"
                occupied:"25%"

            }
            ListElement {
                latitude: 45.420504
                longitude: 11.8505977
                account: "rms1qddwwe4r45ddfdd333344ffttddsskfkkt"
                score:"1.5/5.0"
                occupied:"5%"
            }
        }
    }

}
