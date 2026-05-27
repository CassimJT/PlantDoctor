import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Popup {
    id: instructionPopUp

    anchors.centerIn: parent
    width: parent.width - 53
    modal: true
    // closePolicy: Popup.NoAutoClose

    // ── Background overlay ───────────────────────────────────
    background: Rectangle {
        color: "#00000000"
    }

    // Dim background
    Rectangle {
        parent: Overlay.overlay
        anchors.fill: parent
        color: "#88000000"
        visible: instructionPopUp.visible
    }

    // ── Main card ────────────────────────────────────────────
    contentItem: Rectangle {
        color: "white"
        radius: 24
        clip: true

        // Dynamic sizing based on content
        implicitHeight: header.height + contentColumn.implicitHeight + buttonRow.height + 68

        // Green header strip
        Rectangle {
            id: header
            anchors { top: parent.top; left: parent.left; right: parent.right }
            height: 64
            radius: 24
            color: "#34c45a"

            // Fix bottom corners
            Rectangle {
                anchors { bottom: parent.bottom; left: parent.left; right: parent.right }
                height: 24; color: "#34c45a"
            }

            Text {
                anchors.centerIn: parent
                text: "Low Confidence Notice"
                color: "white"
                font.pointSize: 15
                font.bold: true
                font.letterSpacing: 0.5
            }
        }

        // ── Instruction Content ───────────────────────────────
        Column {
            id: contentColumn
            anchors {
                top: header.bottom
                left: parent.left; right: parent.right
                margins: 24
                topMargin: 24
            }
            spacing: 16

            Text {
                text: "The confidence is low. Please try taking the following steps:"
                color: "#1a2e1a"
                font.pointSize: 13
                font.bold: true
                wrapMode: Text.WordWrap
                width: parent.width
            }

            // Step 1
            RowLayout {
                width: parent.width
                spacing: 12
                Rectangle {
                    width: 24; height: 24; radius: 12; color: "#e8f0e4"
                    Text { text: "1"; anchors.centerIn: parent; color: "#34c45a"; font.bold: true }
                }
                Text {
                    text: "Retake a clear picture"
                    color: "#334433"
                    font.pointSize: 12
                    Layout.fillWidth: true
                    wrapMode: Text.WordWrap
                }
            }

            // Step 2
            RowLayout {
                width: parent.width
                spacing: 12
                Rectangle {
                    width: 24; height: 24; radius: 12; color: "#e8f0e4"
                    Text { text: "2"; anchors.centerIn: parent; color: "#34c45a"; font.bold: true }
                }
                Text {
                    text: "Upload a more clear picture"
                    color: "#334433"
                    font.pointSize: 12
                    Layout.fillWidth: true
                    wrapMode: Text.WordWrap
                }
            }

            // Divider Line
            Rectangle {
                width: parent.width
                height: 1
                color: "#e8ede6"
            }

            // Warning/Hotline Note
            Text {
                text: "If this persists, contact an expert via the hotlines."
                color: "#667766"
                font.pointSize: 12
                font.italic: true
                wrapMode: Text.WordWrap
                width: parent.width
            }
        }

        // ── Action Button ─────────────────────────────────────
        RowLayout {
            id: buttonRow
            anchors {
                bottom: parent.bottom; left: parent.left; right: parent.right
                margins: 24; bottomMargin: 20
            }

            Rectangle {
                Layout.fillWidth: true; height: 48; radius: 24
                gradient: Gradient {
                    orientation: Gradient.Horizontal
                    GradientStop { position: 0.0; color: closeArea.containsPress ? "#2a9e48" : "#34c45a" }
                    GradientStop { position: 1.0; color: closeArea.containsPress ? "#3dbf60" : "#5dde7a" }
                }

                Text {
                    anchors.centerIn: parent
                    text: "Got it"
                    color: "white"
                    font.pointSize: 13
                    font.bold: true
                }

                scale: closeArea.containsPress ? 0.97 : 1.0
                Behavior on scale { NumberAnimation { duration: 100 } }

                MouseArea {
                    id: closeArea
                    anchors.fill: parent
                    onClicked: instructionPopUp.close()
                }
            }
        }
    }
}