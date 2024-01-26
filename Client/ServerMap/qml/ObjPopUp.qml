import QtQuick.Controls
import QtQuick.Layouts
import QtQuick
import Esterv.Styles.Simple
Popup {
    id: control
    signal selected()
    signal showDirections()

    required property real score;
    required property int occupied;

    GridLayout
    {
        columns:2
        anchors.fill: parent
        anchors.margins: 5
        Label
        {
            text:qsTr("Score:")
            Layout.fillWidth: true
        }
        Text
        {
            text: control.score+"/5.0"
            Layout.fillWidth: true
            color:Style.frontColor1
        }
        Label
        {
            text:qsTr("Occupied:")
            Layout.fillWidth: true
        }
        Text
        {
            text: control.occupied+"%"
            Layout.fillWidth: true
            color:Style.frontColor1
        }
        Button
        {
            Layout.fillWidth: true
            text: qsTr("Details")
            onClicked:
            {
                control.selected();
                control.close();
            }
        }
        Button
        {
            Layout.fillWidth: true
            text: qsTr("Directions")
            onClicked:
            {
                control.showDirections();
                control.close();
            }
        }
    }
}
