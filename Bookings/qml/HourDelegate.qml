import QtQuick 2.0
import QtQuick.Controls
import QtQuick.Layouts
import booking_model
import MyDesigns


RowLayout
{    
    id:root_box

    required property bool can_book
    required property bool booked
    required property bool selected
    required property bool sentbook
    required property string hour
    required property int index


    spacing:4
    Text {
        id:time_
        text: root_box.hour
        color:"white"
        Layout.preferredWidth: 80
        Layout.minimumWidth: 50
        Layout.fillHeight: true
        Layout.maximumWidth: 200
        Layout.alignment: Qt.AlignTop
        horizontalAlignment: Text.AlignRight
    }
    Rectangle
    {
        id:line_
        Layout.preferredWidth: 25
        Layout.preferredHeight:  2
        Layout.maximumHeight:   20
        Layout.fillWidth: true
        Layout.alignment: Qt.AlignTop
        border.color:'white'
        border.width: 1
        color:'white'
    }

    Rectangle
    {
        Layout.preferredWidth: 150
        Layout.fillHeight: true
        Layout.fillWidth: true
        Layout.alignment: Qt.AlignHCenter
        Layout.maximumWidth: 400
        border.color:'white'
        border.width: 1
        height:parent.height
        color: root_box.booked?'#e7001b':((root_box.selected?'#0f79af':'transparent'))
        opacity:0.7
        Rectangle
        {
            id: sent_
            color:'#BCAF4D'
            width:0.3*parent.width
            height: parent.height
            opacity:0.5
            visible: root_box.sentbook
        }

        Text
        {
            anchors.centerIn: parent
            text:(root_box.selected)?'-':'+'
            font.pointSize:20
            visible:((!root_box.booked)&&root_box.can_book)
            color:"white"
        }

        MouseArea {
            anchors.fill: parent
            enabled:root_box.can_book
            onClicked: {
                if(!root_box.booked&&!root_box.sentbook)
                {
                    root_box.ListView.view.model.setProperty(root_box.index,"selected",!root_box.selected)
                }

            }
        }
    }




}
