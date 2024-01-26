import QtQuick 2.0
import QtQuick.Layouts
import QtQuick.Controls
import Esterv.Styles.Simple
import Esterv.Dlockers.Bookings
import Esterv.DLockers.Client

ColumnLayout
{
    id:control
    OpenBox
    {
        id:openbox
        visible:false
        closePolicy: Popup.CloseOnPressOutside
    }

    Label
    {
        id:noserver
        visible:!(BookClient.selected);
        opacity:0.5
        Layout.fillHeight: true
        Layout.fillWidth: true
        color: Style.frontColor1
        font.pointSize:250
        fontSizeMode:Text.Fit
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        text: qsTr("Select the server")
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
            enabled:BookClient.selected&&BookClient.selected.dayModel.totalSelected
            onClicked:BookClient.selected.sendBookings();
            visible:BookClient.selected;

        }
        Button
        {
            id:open
            Layout.alignment: Qt.AlignCenter
            Layout.margins: 5
            text: qsTr("Open")
            onClicked:openbox.open()
            visible:BookClient.selected;

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
        visible:BookClient.selected;
    }
    DaySwipeView {
        id: dayview
        clip:true
        canBook:true
        daymodel:BookClient.selected.dayModel
        Layout.fillWidth: true
        Layout.fillHeight:  true
        Layout.minimumWidth: 250
        Layout.maximumWidth: 500
        Layout.alignment: Qt.AlignTop|Qt.AlignHCenter
        visible:BookClient.selected;
    }


}
