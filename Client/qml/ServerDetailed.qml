import QtQuick 2.0
import QtQuick.Layouts
import QtQuick.Controls
import Esterv.Styles.Simple
import Esterv.Dlockers.Bookings
import Esterv.DLockers.Client
import Esterv.Iota.AddrBundle
import Esterv.CustomControls.QrGen
import Esterv.Iota.Wallet

ColumnLayout
{
    id:control
    required property string account;
    required property DayModel dayModel;
    required property int index;
    required property Qml64 price;

    OpenBox
    {
        id:openbox
        visible:false
        closePolicy: Popup.CloseOnPressOutside
        onPresent: (address) =>
                   {
                       console.log("onPresent:",address);
                       BookClient.presentNFT(control.index,address);
                   }
    }

    RowLayout
    {
        Layout.fillWidth: true
        Layout.maximumWidth: 250
        Layout.margins: 5
        Layout.alignment: Qt.AlignHCenter|Qt.AlignTop

        Label
        {
            id:accl
            text:qsTr("Server Id:")
            horizontalAlignment:Text.AlignRight
        }
        QrText
        {
            text: control.account
            Layout.fillWidth: true
            Layout.maximumWidth: implicitWidth
            horizontalAlignment:Text.AlignLeft
        }
    }
    RowLayout
    {
        Layout.alignment: Qt.AlignCenter
        Button
        {
            id:book
            Layout.alignment: Qt.AlignCenter
            Layout.margins: 5
            text: qsTr("Book")
            enabled:control.dayModel.totalSelected&&((Object.keys(Wallet.amount.json).length != 0)&&Wallet.amount.json.largeValue.value>0)
            onClicked: BookClient.sendBookings(control.index)
        }
        Button
        {
            id:open
            Layout.alignment: Qt.AlignCenter
            Layout.margins: 5
            text: qsTr("Open")
            onClicked:openbox.open()
        }

    }
    Item
    {
        Layout.fillWidth: true
        Layout.fillHeight: true
        Layout.maximumHeight: 50
        visible:control.price.json.largeValue.value>0
        RowLayout
        {

            anchors.centerIn: parent
            Label
            {
                Layout.alignment: Qt.AlignCenter
                font:Style.h3
                text:qsTr("Price: ")
                horizontalAlignment: Text.AlignRight
            }
            TextAmount
            {
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignCenter
                font:Style.h2
                amount:control.price
                horizontalAlignment:Text.AlignLeft
            }
        }
    }


    Rectangle
    {
        id:line
        Layout.alignment: Qt.AlignHCenter|Qt.AlignBottom
        Layout.maximumHeight: 2
        Layout.fillHeight: true
        Layout.fillWidth: true
        color: Style.frontColor1
    }

    DaySwipeView {
        id: dayview
        clip:true
        canBook:true
        dayModel:control.dayModel
        Layout.fillWidth: true
        Layout.fillHeight:  true
        Layout.minimumWidth: 250
        Layout.maximumWidth: 500
        Layout.alignment: Qt.AlignTop|Qt.AlignHCenter

    }


}
