import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Page {
    objectName: "SignUp"
    width: 360
    height: 800
    background: Rectangle { color: "#e8efe6" }

    Flickable {
        anchors.fill: parent
        contentHeight: mainColumn.implicitHeight + 60
        clip: true

        ColumnLayout {
            id: mainColumn
            width: parent.width
            spacing: 0

            Item { Layout.preferredHeight: 72 }

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

            Item { Layout.preferredHeight: 60 }

            // ── Card ─────────────────────────────────────────────────────────
            Rectangle {
                Layout.leftMargin: 20
                Layout.rightMargin: 20
                Layout.fillWidth: true
                implicitHeight: cardColumn.implicitHeight + 36
                radius: 20
                color: "#FFFFFF"
                border.color: "#000000"
                border.width: 1

                ColumnLayout {
                    id: cardColumn
                    anchors {
                        top: parent.top; left: parent.left; right: parent.right
                        topMargin: 28; leftMargin: 20; rightMargin: 20
                    }
                    spacing: 16

                    Rectangle {
                        Layout.fillWidth: true
                        height: 56
                        radius: 12
                        color: "#FAFAFA"
                        border.color: nameField.activeFocus ? "#8FAF8F" : "#000000"
                        border.width: nameField.activeFocus ? 1.5 : 1

                        Behavior on border.color { ColorAnimation { duration: 150 } }

                        Text {
                            text: "Full Name"
                            color: "#000000"
                            font.pixelSize: 14
                            anchors.centerIn: parent
                            visible: nameField.text.length === 0 && !nameField.activeFocus
                        }

                        TextField {
                            id: nameField
                            anchors {
                                fill: parent
                                leftMargin: 16; rightMargin: 14
                            }
                            placeholderText: ""
                            color: "#000000"
                            font.pixelSize: 14
                            background: Item {}
                            verticalAlignment: TextInput.AlignVCenter
                        }
                    }

                }
            }
        }
    }
}
