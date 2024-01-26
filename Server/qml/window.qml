import QtQuick.Controls
import QtQuick
import QtQuick.Layouts

import Esterv.Styles.Simple
import Esterv.CustomControls.QrGen
import Esterv.Iota.Wallet
import Esterv.Dlockers.Bookings
import Esterv.DLockers.Server
import Esterv.CustomControls.OpenMeteo

ApplicationWindow {
    id:window
    visible: true
    background: Rectangle
    {
        color:Style.backColor1
    }
    FontLoader {
        id: webFont
        source: "qrc:/esterVtech.com/imports/Esterv/DLockers/Server/qml/fonts/DeliciousHandrawn-Regular.ttf"
    }
    Component.onCompleted:
    {
        Style.h1=Qt.font({
                             family: webFont.font.family,
                             weight: webFont.font.weight,
                             pixelSize: 35
                         });
        Style.h2=Qt.font({
                             family: webFont.font.family,
                             weight: webFont.font.weight,
                             pixelSize: 28
                         });
    }


    ConfDrawer
    {
        id:drawer
        width: Math.max(parent.width*0.2,350)
        height: window.height
    }

    ColumnLayout
    {
        anchors.fill: parent
        Button
        {
            id:openbut
            property bool inOpenBox:false
            Layout.alignment: Qt.AlignCenter
            Layout.margins: 5
            text: qsTr((openbut.inOpenBox)?"Back":"Open")
            onClicked:
            {
                if(!openbut.inOpenBox)
                {
                    BookServer.getNewOpenAddress();
                    goback.start();
                }


                openbut.inOpenBox=!openbut.inOpenBox;
            }
            Timer {
                id:goback
                    interval: 60000; running: false; repeat: false
                    onTriggered: openbut.inOpenBox=false;
                }
        }
        RowLayout
        {
            Layout.fillWidth: true
            Layout.minimumWidth: 200
            Layout.maximumWidth: 350
            Layout.margins: 10
            Layout.alignment: Qt.AlignHCenter|Qt.AlignTop
            Label
            {
                id:accl
                text:qsTr("Account:")
                Layout.alignment: Qt.AlignHCenter|Qt.AlignTop
                horizontalAlignment:Text.AlignRight
                visible:Wallet.addresses.length
            }
            QrText
            {
                text:Wallet.addresses.length?Wallet.addresses[0].bech32Address:""
                visible:Wallet.addresses.length
                Layout.fillWidth: true
                Layout.maximumWidth: implicitWidth
                Layout.alignment: Qt.AlignHCenter|Qt.AlignTop
            }
        }
        Rectangle
        {
            id:line
            Layout.alignment: Qt.AlignHCenter|Qt.AlignBottom
            Layout.preferredHeight: 2
            Layout.fillWidth: true
            color: Style.frontColor1
        }
        RowLayout
        {
            Layout.fillWidth: true
            Layout.fillHeight:  true
            Layout.margins: 10
            Layout.alignment: Qt.AlignCenter
            Layout.maximumWidth: 800
            DaySwipeView
            {
                id: dayview
                clip:true
                canBook:false
                daymodel: BookServer.dayModel
                Layout.fillWidth: true
                Layout.fillHeight:  true
                Layout.minimumWidth: 250
                Layout.alignment: Qt.AlignCenter
                visible:!openbut.inOpenBox
            }
            OpenBox
            {
                id: openbox
                Layout.fillWidth: true
                Layout.fillHeight:  true
                Layout.minimumWidth: 250
                Layout.alignment: Qt.AlignCenter
                visible:openbut.inOpenBox
            }

            CurrentWeather
            {
                Layout.fillWidth: true
                Layout.fillHeight:  true
                Layout.minimumWidth: 150
                Layout.minimumHeight: 150
                Layout.maximumHeight: width
                Layout.alignment: Qt.AlignCenter
                latitude:BookServer.GeoCoord.latitude
                longitude:BookServer.GeoCoord.longitude
                color:Style.frontColor2
            }
        }



    }
}

