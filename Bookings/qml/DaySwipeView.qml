pragma ComponentBehavior: Bound
import QtQuick 2.0
import QtQuick.Controls
import Esterv.Dlockers.Bookings


SwipeView {
    id: control
    required property bool canBook
    required property DayModel dayModel

    currentIndex: 0
    Repeater {
        model: control.dayModel

        delegate: DayDelegate {
            canBook:control.canBook
            width:control.width
            height:control.height
        }
    }


}



