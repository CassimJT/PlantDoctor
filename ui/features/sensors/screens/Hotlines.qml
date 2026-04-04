import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Page {
    objectName: "Hotline"
    width: 360
    height: 800
    background: Rectangle { color: "#e8efe6" }

    ListModel {
        id: contactModel

        ListElement {
            name: "Cassim Twaibu"
            role: "Project manager "
            phone: "+265 883 56 07 59"
            whatsapp: "+265 883 56 07 59"
        }
        ListElement {
            name: "Lisbeth Kumakanga"
            role: "Plant Pathologist"
            phone: "+265 993 52 11 37"
            whatsapp: "+265 993 52 11 37"
        }
        ListElement {
            name: "Daud Osman"
            role: "Agronomist"
            phone: "+265 988 65 08 54"
            whatsapp: "+265 988 65 08 54"
        }
        ListElement {
            name: "Noel Gonthi"
            role: "Soil Scientist"
            phone: "+265 997 46 48 88"
            whatsapp: "+265 997 46 48 88"
        }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Item { Layout.preferredHeight: 60 }

        // ── Heading ──────────────────────────────────────────────────────────
        Text {
            Layout.alignment: Qt.AlignHCenter
            text: "Get In Touch!"
            font.family: "Georgia"
            font.pixelSize: 28
            font.bold: true
            color: "#1A2E1F"
            font.letterSpacing: 0.3
        }
        Item { Layout.preferredHeight: 6 }

        Text {
            Layout.alignment: Qt.AlignHCenter
            text: "Tap to call or message via WhatsApp"
            font.pixelSize: 13
            color: "#5A7A5E"
        }
        Item { Layout.preferredHeight: parent.height * 0.08 }

        // ── Contacts list ────────────────────────────────────────────────────
        Flickable {
            Layout.fillWidth: true
            Layout.preferredHeight: contactsColumn.implicitHeight + 40
            contentHeight: contactsColumn.implicitHeight + 40
            clip: true

            ColumnLayout {
                id: contactsColumn
                width: parent.width
                spacing: 10

                Repeater {
                    model: contactModel

                    delegate: Rectangle {
                        Layout.leftMargin: 20
                        Layout.rightMargin: 20
                        Layout.fillWidth: true
                        height: 80
                        radius: 16
                        color: "#FFFFFF"
                        border.color: "#000000"
                        border.width: 1

                        RowLayout {
                            anchors {
                                fill: parent
                                leftMargin: 12
                                rightMargin: 12
                            }
                            spacing: 10

                            // ── Avatar ───────────────────────────────────────
                            Rectangle {
                                width: 42; height: 42
                                radius: 21
                                color: "#8FAF8F"

                                Text {
                                    anchors.centerIn: parent
                                    text: name.charAt(0)
                                    font.pixelSize: 18
                                    font.bold: true
                                    color: "#FFFFFF"
                                }
                            }

                            // ── Name, role, phone ────────────────────────────
                            ColumnLayout {
                                Layout.fillWidth: true
                                spacing: 2

                                Text {
                                    text: name
                                    font.pixelSize: 14
                                    font.bold: true
                                    color: "#1A2E1F"
                                }
                                Text {
                                    text: role
                                    font.pixelSize: 11
                                    color: "#5A7A5E"
                                }
                                Text {
                                    text: phone
                                    font.pixelSize: 11
                                    color: "#0A66C2"
                                    font.underline: true

                                    MouseArea {
                                        anchors.fill: parent
                                        onClicked: Qt.openUrlExternally("tel:" + phone)
                                    }
                                }
                            }

                            // ── Call button ──────────────────────────────────
                            ColumnLayout {
                                spacing: 2
                                Layout.alignment: Qt.AlignHCenter

                                Rectangle {
                                    Layout.alignment: Qt.AlignHCenter
                                    width: 34; height: 34
                                    radius: 17
                                    color: callMA.pressed ? "#b0ccb0" : "#E8F0E8"
                                    border.color: "#000000"
                                    border.width: 1

                                    Behavior on color { ColorAnimation { duration: 100 } }

                                    Text {
                                        anchors.centerIn: parent
                                        text: "📞"
                                        font.pixelSize: 14
                                    }

                                    MouseArea {
                                        id: callMA
                                        anchors.fill: parent
                                        onClicked: Qt.openUrlExternally("tel:" + phone)
                                    }
                                }

                                Text {
                                    Layout.alignment: Qt.AlignHCenter
                                    text: "Call"
                                    font.pixelSize: 10
                                    color: "#1A2E1F"
                                }
                            }

                            // ── WhatsApp button ──────────────────────────────
                            ColumnLayout {
                                spacing: 2
                                Layout.alignment: Qt.AlignHCenter

                                Rectangle {
                                    Layout.alignment: Qt.AlignHCenter
                                    width: 34; height: 34
                                    radius: 17
                                    color: waMA.pressed ? "#1da84f" : "#25D366"

                                    Behavior on color { ColorAnimation { duration: 100 } }

                                    Text {
                                        anchors.centerIn: parent
                                        text: "\u260E"
                                        font.pixelSize: 16
                                        color: "#FFFFFF"
                                    }

                                    MouseArea {
                                        id: waMA
                                        anchors.fill: parent
                                        onClicked: Qt.openUrlExternally("https://wa.me/" + whatsapp)
                                    }
                                }

                                Text {
                                    Layout.alignment: Qt.AlignHCenter
                                    text: "WhatsApp"
                                    font.pixelSize: 10
                                    color: "#1A2E1F"
                                }
                            }
                        }
                    }
                }

                Item { Layout.preferredHeight: 20 }
            }
        }

        Item { Layout.preferredHeight: parent.height * 0.15 }
    }
}




