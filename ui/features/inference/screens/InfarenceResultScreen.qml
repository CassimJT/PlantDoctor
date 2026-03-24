import QtQuick 2.15
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: inferenceScreen
    property string imagePath: ""   // receives capturedImagePath

    // use it like:
    Rectangle{
        anchors.fill: parent
        color: "red"
        Image {
            source: "file://" + imagePath
            width: 100
            height: 200
        }
    }


}
