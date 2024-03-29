
import QtQuick 2.0
import QtQuick.Layouts
import QtQuick.Controls
import Esterv.DLockers.Client
import Esterv.CustomControls.QrDec
import Esterv.Iota.AddrBundle


Popup
{
    id:control
    anchors.centerIn: Overlay.overlay
    signal present( string address)

    onClosed: recAddress.text=""
    ColumnLayout
    {
        Label
        {
            text:qsTr("Address:")
        }
        QrTextField
        {
            id:recAddress
            Layout.fillWidth: true
            Layout.minimumWidth: 200
            AddressChecker
            {
                id:recAddrCheck
                address:recAddress.text
            }
            ToolTip.text: qsTr("Not an address")
            ToolTip.visible: (!recAddrCheck.valid)&&(recAddrCheck.address!=="")
        }
        Button
        {
            text:qsTr("Ok")
            enabled:recAddrCheck.valid
            onClicked:
            {
                control.present(recAddress.text);
                control.close();                
            }
            Layout.alignment: Qt.AlignRight
        }
    }
}

