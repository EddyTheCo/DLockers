import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import Esterv.CustomControls.QrGen
import Esterv.Styles.Simple
import Esterv.Iota.AddrBundle
import Esterv.Iota.Wallet


Rectangle
{
    id:root
    property bool showMapO: false
    signal showSettings()
    signal showMap()
    color:Style.backColor2
    radius:3

    ColumnLayout
    {
        anchors.fill: parent
        anchors.margins: 10
        RowLayout
        {
            Layout.fillWidth: true
            Layout.minimumWidth: 300


            Label
            {
                id:accl
                text:qsTr("Account:")
                Layout.alignment: Qt.AlignHCenter|Qt.AlignTop
                horizontalAlignment:Text.AlignRight
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

        Label
        {
            Layout.alignment: Qt.AlignHCenter|Qt.AlignTop
            font:Style.h3
            text:qsTr("Balance: ")
        }
        TextAmount
        {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignHCenter|Qt.AlignTop
            font:Style.h2
            amount:Wallet.amount
            horizontalAlignment:Text.AlignHCenter
        }

        GridLayout
        {
            rowSpacing: 5
            columnSpacing: 15
            Layout.margins: 5
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignCenter
            Layout.minimumHeight: 45
            flow: GridLayout.LeftToRight
            Button
            {
                id:settings_
                text:qsTr("Settings")
                onReleased:root.showSettings();                
                ToolTip.text: text
                ToolTip.visible: hovered
            }
            Button
            {
                id:map
                text:qsTr("Map")
                onClicked:root.showMap();
                visible:root.showMapO
                ToolTip.text: text
                ToolTip.visible: hovered
            }
        }

    }


}


