pragma ComponentBehavior: Bound

import QtQuick.Controls
import QtQuick
import QtQuick.Layouts
import Esterv.Styles.Simple
import Esterv.Iota.NodeConnection
import Esterv.Iota.Account
import Esterv.CustomControls.CoordToMap
import Esterv.Iota.DLockerClient


ApplicationWindow {
    visible: true
    id:window
    FontLoader {
        id: webFont
        source: "qrc:/esterVtech.com/imports/Esterv/Iota/DLockerClient/qml/fonts/DeliciousHandrawn-Regular.ttf"
    }
    Component.onCompleted:
    {
        Style.h1=Qt.font({
                             family: webFont.font.family,
                             weight: webFont.font.weight,
                             pixelSize: 28
                         });
        Style.h2=Qt.font({
                             family: webFont.font.family,
                             weight: webFont.font.weight,
                             pixelSize: 28
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

    ObjectMapView
    {
        anchors.fill: parent
        objModel: Book_Client
    }

}



