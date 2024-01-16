import QtQuick.Controls
import QtQuick
import QtQuick.Layouts
import Esterv.Iota.Account
import Esterv.Styles.Simple
import Esterv.Iota.NodeConnection
import Esterv.CustomControls

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

        NodeConnectionSettings
        {
            id:conn_
            Layout.fillWidth: true
        }
        AccountSettings
        {
            id:acc_
            Layout.fillWidth: true
        }
    }

}
