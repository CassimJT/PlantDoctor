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
            name: "John Doe"
            role: "Agricultural Officer"
            phone: "+265991000001"
            whatsapp: "+265991000001"
        }
        ListElement {
            name: "Jane Smith"
            role: "Plant Pathologist"
            phone: "+265991000002"
            whatsapp: "+265991000002"
        }
        ListElement {
            name: "David Banda"
            role: "Agronomist"
            phone: "+265991000003"
            whatsapp: "+265991000003"
        }
        ListElement {
            name: "Grace Phiri"
            role: "Soil Scientist"
            phone: "+265991000004"
            whatsapp: "+265991000004"
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

    }

}
