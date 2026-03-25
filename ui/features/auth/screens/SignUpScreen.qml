import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Page {
    objectName: "SignUp"
    width: 360
    height: 800
    background: Rectangle { color: "#e8efe6" }

    Text {
        anchors.horizontalCenter: parent.horizontalCenter
        y: 72
        text: "Create Account"
        font.family: "Georgia"
        font.pixelSize: 26
        color: "#1A2E1F"
        font.bold: true
        font.letterSpacing: 0.3
    }
}
