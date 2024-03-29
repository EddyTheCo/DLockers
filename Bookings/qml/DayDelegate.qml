import QtQuick 2.0
import Esterv.Dlockers.Bookings
import Esterv.Styles.Simple

Item
{

    id:control
    required property  date day
    required property  HourModel hourModel
    required property  bool canBook



    Rectangle
    {

        id:headlabel

        color: Style.backColor2
        width:parent.width
        height:parent.height*0.2

        Text
        {
            width:parent.width
            height:parent.height*0.65
            text: control.day.toLocaleString(Qt.locale(),"ddd")
            color: Style.frontColor1
            font.family: Style.h1.family
            font.pointSize:250
            fontSizeMode:Text.Fit
            horizontalAlignment: Text.AlignHCenter
        }
        Text
        {
            width:parent.width
            height:parent.height*0.3
            anchors.bottom: parent.bottom
            text: control.day.toLocaleString(Qt.locale(),"dd/MM/yy")
            font.family: Style.h1.family
            font.pointSize:250
            color: Style.frontColor1
            fontSizeMode:Text.Fit
            horizontalAlignment: Text.AlignHCenter
        }

    }
    HourListView
    {
        id:hourlistview
        anchors.top: headlabel.bottom
        width:parent.width
        height:parent.height*0.8
        canBook:control.canBook
        hourModel:control.hourModel
    }






}


