import QtQuick.Controls
import QtQuick
import QtQuick.Layouts
import Esterv.Iota.Account
import Esterv.Styles.Simple
import Esterv.Iota.NodeConnection
import Esterv.CustomControls
import Esterv.Iota.AddrBundle
import Esterv.Iota.Wallet

Drawer
{
    id: drawer
    closePolicy: Popup.CloseOnPressOutside
    focus:true
    modal:true
    background: Rectangle
    {
        color:Style.backColor2
    }

    ColumnLayout {
        anchors.fill: parent
        Layout.margins: 5
        ThemeSwitch
        {

        }


        TextAmount
        {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignHCenter|Qt.AlignTop
            font:Style.h1
            amount:Wallet.amount
            horizontalAlignment:Text.AlignHCenter
        }

        NodeConnectionSettings
        {
            id:conn_
            Layout.fillWidth: true
        }

    }

}
