import QtQuick 2.0
import QtQuick.Layouts
import booking_model
import MyDesigns

ColumnLayout
{

    id:dia_delegate
    required property  date day
    required property  Hour_model hour_model
    required property  bool can_book

    Rectangle
    {

        id:day_label

        Layout.minimumHeight:  85
        Layout.maximumHeight:  100
        Layout.maximumWidth:  400
        Layout.fillWidth: true
        Layout.fillHeight: true

        Layout.alignment: Qt.AlignHCenter
        color: '#0f79af'
        ColumnLayout
        {

            anchors.fill: day_label
            Text
            {

                text: dia_delegate.day.toLocaleString(Qt.locale(),"ddd")
                color:"white"
                font.pointSize:30
                Layout.minimumHeight:  50
                Layout.minimumWidth:  100
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.alignment: Qt.AlignCenter
                horizontalAlignment: Text.AlignHCenter
            }
            Text
            {
                text: dia_delegate.day.toLocaleString(Qt.locale(),"dd/MM/yy")
                font.pointSize:20
                color:"white"
                Layout.minimumHeight:  25
                Layout.minimumWidth:  100
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.alignment: Qt.AlignCenter
                horizontalAlignment: Text.AlignHCenter
            }

        }

    }
    Horario_list_view
    {
        id:hours_container

        Layout.fillHeight: true

        Layout.minimumHeight:  400
        Layout.maximumWidth:  400
        Layout.fillWidth: true
        Layout.alignment: Qt.AlignHCenter

        can_book:dia_delegate.can_book
        horario_model: dia_delegate.hour_model

    }






}


