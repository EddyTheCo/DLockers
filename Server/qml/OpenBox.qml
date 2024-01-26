import QtQuick 2.0
import QtQuick.Layouts
import QtQuick.Controls
import Esterv.DLockers.Server
import Esterv.Styles.Simple
import Esterv.CustomControls.QrGen

Item
{
    ColumnLayout
    {
        anchors.fill: parent
        Label
        {
            Layout.fillWidth: true
            text:qsTr("Send the nft to:")
            font:Style.h1
            horizontalAlignment:Text.AlignLeft
            fontSizeMode:Text.Fit
            Layout.alignment: Qt.AlignTop
        }
        QrText
        {
            id:nftaddr
            Layout.fillWidth: true
            text: BookServer.openAddress
            font:Style.h2
            Layout.alignment: Qt.AlignTop
            Layout.bottomMargin: 10
            visible:BookServer.openAddress!==""
        }
        Rectangle
        {
            color:Style.backColor2
            Layout.fillHeight:  true
            border.width:2
            border.color:Style.frontColor1
            Layout.fillWidth: true
            Layout.maximumWidth: parent.width*0.8
            Layout.minimumHeight: 50
            Layout.maximumHeight: 200
            radius:Math.min(width,height)*0.07
            Layout.alignment: Qt.AlignCenter
            Layout.margins: 30
            Label
            {
                anchors.centerIn: parent
                width: parent.width*0.85
                height:parent.height
                text:qsTr("The locker is %1").arg(BookServer.open?"open":"closed")
                font:Style.h1
                horizontalAlignment:Text.AlignHCenter
                verticalAlignment:Text.AlignVCenter
                fontSizeMode:Text.Fit
            }
        }

    }

}







