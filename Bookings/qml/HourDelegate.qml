import QtQuick 2.0
import QtQuick.Controls
import QtQuick.Layouts
import Esterv.Dlockers.Bookings
import Esterv.Styles.Simple


RowLayout
{    
    id:control

    required property bool canBook
    required property bool booked
    required property bool selected
    required property bool sentbook
    required property int hour
    required property int index

    spacing:4
    Text {
        id:time_
        text: new Date('December 17, 1995 '+root_box.hour+':00:00').toLocaleTimeString(Qt.locale(),"h a");
        color: Style.frontcolor1
        Layout.minimumWidth: 50
        Layout.fillHeight: true
        Layout.fillWidth: true
        horizontalAlignment: Text.AlignRight
        verticalAlignment: Text.AlignTop
        font.pointSize:250
        fontSizeMode:Text.Fit
    }
    Rectangle
    {
        id:line_
        Layout.preferredWidth: 25
        Layout.minimumWidth: 5
        Layout.preferredHeight:  3
        Layout.fillWidth: true
        Layout.alignment: Qt.AlignTop
        color:Style.frontcolor2
    }

    Rectangle
    {
        Layout.preferredWidth: 150
        Layout.fillHeight: true
        Layout.fillWidth: true
        Layout.maximumWidth: 400
        border.color:Style.frontcolor2
        border.width: 1
        color: control.booked?'#e7001b':((root_box.selected?'#0f79af':'transparent'))
        opacity:0.7
        Rectangle
        {
            id: sent_
            color:'#BCAF4D'
            width:0.3*parent.width
            height: parent.height
            opacity:0.5
            visible: control.sentbook
        }

        Text
        {
            anchors.centerIn: parent
            text:(control.selected)?'-':'+'
            font.pointSize:20
            visible:((!control.booked)&&control.can_book)
            color:Style.frontcolor1
        }

        MouseArea {
            anchors.fill: parent
            enabled:control.can_book
            onClicked: {
                if(!control.booked&&!control.sentbook)
                {
                    control.ListView.view.model.setProperty(control.index,"selected",!control.selected)
                }
            }
        }
    }




}
