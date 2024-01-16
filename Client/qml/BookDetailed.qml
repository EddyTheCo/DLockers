import QtQuick 2.0
import QtQuick.Layouts
import QtQuick.Controls
import Esterv.Styles.Simple
import booking_model

ColumnLayout
{
    id:control

    Button
    {
        id:book
        Layout.alignment: Qt.AlignCenter
        Layout.margins: 5
        text: qsTr("Book")
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
    Day_swipe_view {
        id: dayview
        clip:true
        can_book:true
        Layout.fillWidth: true
        Layout.fillHeight:  true
        Layout.minimumWidth: 300
        Layout.maximumWidth: 700
        Layout.alignment: Qt.AlignTop|Qt.AlignHCenter
    }
}
