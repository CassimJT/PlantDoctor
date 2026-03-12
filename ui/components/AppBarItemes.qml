import QtQuick 2.15
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    anchors.fill: parent
    color: Qt.rgba(0,0,0,0)
    ColumnLayout {
        anchors.centerIn: parent
        width: parent.width
        spacing: 3

        RowLayout{
            Layout.preferredWidth: parent.width
            Label {
                text: "CCTV"
                Layout.alignment: Qt.AlignLeft
                Layout.leftMargin: 20

            }
            Switch {
                id: camera
                text: camera.checked ? "ON" : "OFF"
                Layout.alignment: Qt.AlignRight
                Layout.rightMargin:  15
            }
        }
        MenuSeparator{
            Layout.preferredWidth: parent.width * 0.9
            Layout.alignment: Qt.AlignHCenter
        }

        RowLayout{
            Layout.preferredWidth: parent.width
            Label {
                text: "Mood"
                Layout.alignment: Qt.AlignLeft
                Layout.leftMargin: 20
            }
            Switch {
                id: mode
                text: mode.checked ? "MAN" : "AUT"
                Layout.alignment: Qt.AlignRight
                Layout.rightMargin:  15
            }
        }
        MenuSeparator{
            Layout.preferredWidth: parent.width * 0.9
            Layout.alignment: Qt.AlignHCenter
        }
        Label {
            text: "RTS URL"
            Layout.leftMargin: 20
        }

        ComboBox {
            Layout.preferredWidth: parent.width * 0.8
            Layout.leftMargin: 20
            editable: true
            model: ['rtsp://192.168.8.117:8554/video']
        }
    }
}
