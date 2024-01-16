import QtQuick 2.0
import QtQuick.Controls
import Esterv.Dlockers.Bookings


SwipeView {
    id: control
    required property bool can_book

    currentIndex: 0

    Repeater {
        model: DayModel
        delegate: DayDelegate {
            can_book:control.can_book
            width:control.width
            height:control.height
        }
    }


}



