pragma ComponentBehavior: Bound

import QtQuick.Controls
import QtQuick
import QtQuick.Layouts
import Esterv.Styles.Simple
import Esterv.Dlockers.Map
import Esterv.DLockers.Client


ApplicationWindow {
    id:window
    visible:true
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

    Connections {
        target: BookClient
        function onSelectedChanged() {
            if(rlt.onecolumn)
            {
                rlt.showmap=false;
            }
        }
        function onOneColumnChanged(){
            if(BookClient.onecolumn)
            {
                rlt.showmap=true;
            }
        }
    }
    RowLayout
    {
        id: rlt
        anchors.fill: parent
        property bool onecolumn:(parent.width<600&&!BookClient.isBrowser)||(BookClient.isBrowser&&BookClient.onecolumn)
        property bool showmap: true
        ListView
        {
            id:serversDetails
            clip:true
            interactive:false
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.maximumWidth: 500
            Layout.minimumWidth: 300
            Layout.alignment: Qt.AlignHCenter|Qt.AlignTop
            visible:!rlt.onecolumn||(rlt.onecolumn&&!rlt.showmap)
            model: BookClient
            currentIndex:BookClient.selected
            delegate:ServerDetailed
            {
                width:serversDetails.width
                height:serversDetails.height
            }


        }

        ObjectMapView
        {
            id:objmapview
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.minimumWidth: 300
            visible:!rlt.onecolumn||(rlt.onecolumn&&rlt.showmap)
        }

    }
    AccountMenu
    {
        width:Math.min(350,parent.width)
        height:200
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        onShowSettings: drawer.open();
        showMapO:rlt.onecolumn&&!rlt.showmap
        onShowMap:
        {
            rlt.showmap=true;
        }
    }


}



