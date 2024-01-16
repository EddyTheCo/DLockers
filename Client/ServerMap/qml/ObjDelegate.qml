import QtLocation
import QtPositioning
import QtQuick
import QtQuick.Controls
MapQuickItem {
            id: control
            required property real latitude;
            required property real longitude;
            required property string account;
            required property real score;
            required property int occupied;
            signal selected(string account)
            signal showDirections(var coords)

            Component.onCompleted:
            {
                console.log("latitude:",control.latitude);
                console.log("longitude:",control.longitude);
                console.log("account:",control.account);
                console.log("score:",control.score);
                console.log("occupied:",control.occupied);
                console.log("coordinate:",control.coordinate);
                console.log("visible:",control.visible);
            }

            coordinate: QtPositioning.coordinate(latitude, longitude)
            autoFadeIn:false
            sourceItem: Rectangle {
                id: image
                width:24
                height:24
                radius:24
                color:"red"
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
               anchors.centerIn: Overlay.overlay
               score:control.score
               occupied:control.occupied

               onSelected: control.selected(control.account);
               onShowDirections: control.showDirections(QtPositioning.coordinate(latitude, longitude));
           }
        }
