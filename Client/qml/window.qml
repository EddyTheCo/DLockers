pragma ComponentBehavior: Bound

import QtQuick.Controls
import QtQuick
import QtQuick.Layouts
import Esterv.Styles.Simple
import Esterv.Dlockers.Map
import Esterv.DLockers.Client


ApplicationWindow {
    visible: true
    id:window
    FontLoader {
        id: webFont
        source: "qrc:/esterVtech.com/imports/Esterv/DLockers/Client/qml/fonts/DeliciousHandrawn-Regular.ttf"
    }
    Component.onCompleted:
    {
        Style.h1=Qt.font({
                             family: webFont.font.family,
                             weight: webFont.font.weight,
                             pixelSize: 60
                         });
        Style.h2=Qt.font({
                             family: webFont.font.family,
                             weight: webFont.font.weight,
                             pixelSize: 30
                         });
    }

    background: Rectangle
    {
        color:Style.backColor1
    }

    ConfDrawer
    {
        id:drawer
        width: Math.max(parent.width*0.2,350)
        height: window.height
    }
    RowLayout
    {
        anchors.fill: parent

        ServerDetailed
        {
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.maximumWidth: 500
            Layout.minimumWidth: 200
            Layout.minimumHeight: 400
            visible:BookClient.selected
        }



        ObjectMapView
        {
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.minimumWidth: 100
            Layout.minimumHeight: 500
            AccountMenu
            {
                width:Math.min(300,parent.width)
                height:250
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: parent.bottom
                onShowSettings: drawer.open();
            }

        }





    }



}



