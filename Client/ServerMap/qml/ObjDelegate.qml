import QtLocation
import QtPositioning
import QtQuick
import QtQuick.Controls
import Esterv.DLockers.Client
MapQuickItem {
            id: control
            required property real latitude;
            required property real longitude;
            required property string account;
            required property real score;
            required property int occupied;
            required property int index;
            signal showDirections(var coords)

            coordinate: QtPositioning.coordinate(latitude, longitude)
            autoFadeIn:false
            sourceItem: Image {
                id: image
                width:24
                height:24
                source: "qrc:/esterVtech.com/imports/Esterv/Dlockers/Map/qml/icons/safety-box.png"
            }
            MouseArea
            {
                anchors.fill: parent
                onClicked: objpop.open()
            }

           ObjPopUp
           {
               id:objpop
               visible:false
               closePolicy:Popup.CloseOnPressOutside
               focus:true
               modal:true
               score:control.score
               occupied:control.occupied

               onSelected: BookClient.setSelected(control.index);
               onShowDirections: control.showDirections(QtPositioning.coordinate(latitude, longitude));
           }
        }
