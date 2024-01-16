import QtQuick 2.0
import QtQuick.Controls
import booking_model

ListView {
    id:horario
    required property Hour_model horario_model
    required property  bool can_book

    clip:true
    flickableDirection:Flickable.VerticalFlick

    model: horario_model
    delegate: Horario_delegate {
        width:ListView.view.width
        height:width*0.2
        can_book:horario.can_book
    }


}
