pragma ComponentBehavior: Bound
import QtQuick 2.0
import Esterv.Dlockers.Bookings

ListView {
    id:control
    required property HourModel hourModel
    required property  bool canBook

    clip:true
    flickableDirection:Flickable.VerticalFlick

    model: control.hourModel

    delegate: HourDelegate {
        width:control.width
        height:width*0.2
        canBook:control.canBook
    }


}
