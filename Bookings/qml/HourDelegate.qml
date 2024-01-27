import QtQuick 2.0
import QtQuick.Controls
import QtQuick.Layouts
import Esterv.Dlockers.Bookings
import Esterv.Styles.Simple


RowLayout
{    
    id:control

    required property bool canBook
    required property int hour
    required property int state
    required property int index

    spacing:4
    Text {
        id:time
        text: (control.hour<12)?(((control.hour)?control.hour:12)+" " + Qt.locale().amText):((control.hour===12)?12:control.hour-12)+" "+ Qt.locale().pmText;
        color: Style.frontColor1
        Layout.preferredWidth: 80
        Layout.minimumWidth: 50
        Layout.fillHeight: true
        Layout.maximumWidth: 200
        Layout.alignment: Qt.AlignTop
        horizontalAlignment: Text.AlignRight

    }
    Rectangle
    {
        id:line
        Layout.preferredWidth: 20
        Layout.preferredHeight:  2
        Layout.maximumWidth: 40
        Layout.fillWidth: true
        Layout.alignment: Qt.AlignTop
        color:Style.frontColor2
        border.color:Style.frontColor2
        border.width: 1
    }

    Rectangle
    {
        Layout.preferredWidth: 150
        Layout.fillHeight: true
        Layout.fillWidth: true
        Layout.alignment: Qt.AlignHCenter
        border.color:Style.frontColor2
        border.width: 1
        color: control.state===HourBox.Selected?Style.backColor3:'transparent'
        opacity:0.7

        Text
        {
            anchors.fill: parent
            text: (control.state===HourBox.Selected?"-":(control.state===HourBox.Occupied)?"xxx":"+")
            fontSizeMode:Text.Fit
            visible:(control.canBook&&(control.state===HourBox.Selected||control.state===HourBox.Free||control.state===HourBox.Occupied))
            color:Style.frontColor1
            verticalAlignment:Text.AlignVCenter
            horizontalAlignment:Text.AlignHCenter
            font:Style.h1
        }
        BusyIndicator {
            anchors.centerIn: parent
            running: control.state===HourBox.Booking
        }
        Rectangle
        {
            anchors.verticalCenter: parent.verticalCenter
            width:parent.width*0.4
            height:parent.height*0.8
            radius:5
            color: Style.backColor3
            border.color: Style.frontColor1
            border.width: 1
            visible: control.state===HourBox.Booked
            Text
            {
                anchors.fill: parent
                anchors.margins: 3
                verticalAlignment:Text.AlignVCenter
                horizontalAlignment:Text.AlignHCenter
                fontSizeMode:Text.Fit
                color:Style.frontColor1
                text: qsTr("Booked")
                font:Style.h2
            }
        }

        MouseArea {
            anchors.fill: parent
            enabled:control.canBook&&(control.state===HourBox.Free||control.state===HourBox.Selected)
            onClicked: {
                    control.ListView.view.model.setProperty(control.index,"state",(control.state===HourBox.Free)?HourBox.Selected:HourBox.Free)
            }
        }
    }




}
