import QtQuick 2.15
import QtQuick.Controls

Rectangle {
    anchors.fill: parent
    color: "#e8efe6"

    Column{

       anchors.fill: parent
        Text {
            id: camera
            text: qsTr("camera screen")
        }

        Rectangle{
            width: 50
            height: 50
            color: "blue"
            MouseArea{
                anchors.fill: parent
                onClicked: StackView.pop()
            }
        }


    }




}
