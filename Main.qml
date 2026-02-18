import QtQuick
import QtQuick.Controls

ApplicationWindow {
    id: window
    width: 640
    height: 480
    visible: true
    title: qsTr("PlantDoctor")
    flags: Qt.FramelessWindowHint | Qt.Window  | Qt.ExpandedClientAreaHint
    visibility: window.FullScreen

    BusyIndicator {
        anchors.centerIn: parent
    }


}
