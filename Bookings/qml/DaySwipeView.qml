import QtQuick 2.0
import QtQuick.Controls
import booking_model


SwipeView {
    id: swipe_view
    required property bool can_book

    currentIndex: 0

    Repeater {
        model: Day_model
        delegate: Dia_delegate {
            can_book:swipe_view.can_book

        }
    }


}



