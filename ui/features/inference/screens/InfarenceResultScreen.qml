import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Page {
    id: inferenceResultScreen

    property string imagePath: ""

    // ── Placeholder data (replace with real inference results) ──
    property string plantName:    "Spinach Plant"
    property string diseaseName:  "Downy Mildew"

    property real   riskLevel:    0.72   // 0.0 = Low, 1.0 = High
    property string description:  "Downy mildew is a common and destructive fungal disease affecting spinach. Symptoms: Yellowish spots appear on the upper leaf surface. A purplish-gray mold develops on the underside of leaves."
    property string noteText:     "Early detection and prompt action are crucial for managing downy mildew. If you suspect downy mildew in your spinach, consult a local agricultural extension agent for specific recommendations."
    property var treatments: [
        { title: "Fungicides",          body: "While fungicides can help control the spread, they cannot cure the disease. Preventive applications are essential. Consult local agricultural experts for recommended fungicides and application timing." },
        { title: "Resistant Varieties", body: "Planting spinach varieties with resistance to downy mildew can significantly reduce the risk of infection." },
        { title: "Crop Rotation",       body: "Avoid planting spinach in the same location year after year to break the disease cycle." }
    ]

    background: Rectangle { color: "#edf2e0" }

    // Fade-in on load
    opacity: 0
    Component.onCompleted: fadeIn.start()
    PropertyAnimation {
        id: fadeIn
        target: inferenceResultScreen
        property: "opacity"
        from: 0; to: 1; duration: 380; easing.type: Easing.OutCubic
    }


    // ── Scrollable content ───────────────────────────────────
    ScrollView {
        anchors {
                top: parent.top
                left: parent.left; right: parent.right
                bottom: bottomBar.top
            }
        contentWidth: availableWidth
        clip: true

        Column {
            width: parent.width
            spacing: 0
            topPadding: 60
            bottomPadding: 24

            // ── Hero card ────────────────────────────────────
            Rectangle {
                width: parent.width - 32
                height: 100
                anchors.horizontalCenter: parent.horizontalCenter
                radius: 16
                color: "#1e2b1e"

                // Drop shadow
                layer.enabled: true
                layer.effect: null

                RowLayout {
                    anchors {
                        fill: parent
                        margins: 14
                    }
                    spacing: 14

                    // Plant thumbnail
                    Rectangle {
                        width: 75; height: 75
                        radius: 20
                        color: "#2d3d2d"
                        clip: true
                        Layout.alignment: Qt.AlignVCenter

                        Image {
                            anchors.fill: parent
                            source: imagePath ? "file://" + imagePath : ""
                            fillMode: Image.PreserveAspectCrop
                            smooth: true
                        }

                        // Fallback leaf icon if no image
                        Text {
                            anchors.centerIn: parent
                            text: "🌿"
                            font.pointSize: 28
                            visible: imagePath === ""
                        }
                    }

                    // Disease info
                    Column {
                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignVCenter
                        spacing: 4

                        Text {
                            text: diseaseName
                            color: "white"
                            font.pointSize: 18
                            font.bold: true
                            font.letterSpacing: 0.2
                        }
                        Text {
                            text: plantName
                            color: "#88aa88"   // ← plain ASCII a's
                            font.pointSize: 12
                            font.letterSpacing: 0.3
                        }
                    }


                }
            }

            // Spacer
            Item { width: 1; height: 20 }

            // ── Section: Disease name ─────────────────────────
            Text {
                text: diseaseName + " on " + plantName
                color: "#1a2e1a"
                font.pointSize: 14
                font.bold: true
                anchors.left: parent.left
                anchors.leftMargin: 20
            }

            Item { width: 1; height: 10 }

            // Description
            Text {
                text: description
                color: "#556655"
                font.pointSize: 11
                wrapMode: Text.WordWrap
                lineHeight: 1.5
                width: parent.width - 40
                anchors.left: parent.left
                anchors.leftMargin: 20
            }

            Item { width: 1; height: 22 }

            // ── Section: Treatment ───────────────────────────
            Text {
                text: "Treatment and Prevention"
                color: "#1a2e1a"
                font.pointSize: 14
                font.bold: true
                anchors.left: parent.left
                anchors.leftMargin: 20
            }

            Item { width: 1; height: 12 }

            // Treatment bullet items
            Repeater {
                model: treatments

                Column {
                    width: parent.width - 40
                    anchors.left: parent.left
                    anchors.leftMargin: 20
                    spacing: 4
                    bottomPadding: 10

                    Row {
                        spacing: 8
                        width: parent.width

                        // Bullet dot
                        Rectangle {
                            width: 6; height: 6; radius: 3
                            color: "#4a7c59"
                            anchors.top: parent.top
                            anchors.topMargin: 5
                        }

                        Text {
                            width: parent.width - 14
                            color: "#334433"
                            font.pointSize: 11
                            wrapMode: Text.WordWrap
                            lineHeight: 1.5

                            text: "<b>" + modelData.title + ":</b> " + modelData.body
                            textFormat: Text.RichText
                        }
                    }
                }
            }

            Item { width: 1; height: 20 }


            // ── Risk level card ───────────────────────────────────
            Rectangle {
                width: parent.width - 32
                anchors.horizontalCenter: parent.horizontalCenter
                height: 90
                radius: 14
                color: "white"
                border.color: "#e0e8de"; border.width: 1

                Column {
                    anchors {
                        fill: parent
                        margins: 16
                    }
                    spacing: 10

                    Text {
                        text: "Risk life prediction"
                        color: "#1a2e1a"
                        font.pointSize: 12
                        font.bold: true
                    }

                    // Progress bar track
                    Rectangle {
                        width: parent.width
                        height: 6
                        radius: 3
                        color: "#e8ede6"

                        Rectangle {
                            width: parent.width * riskLevel
                            height: parent.height
                            radius: 3
                            gradient: Gradient {
                                orientation: Gradient.Horizontal
                                GradientStop { position: 0.0; color: "#6dbf7e" }
                                GradientStop { position: 0.6; color: "#e8a838" }
                                GradientStop { position: 1.0; color: "#d64e3a" }
                            }
                            Behavior on width { NumberAnimation { duration: 800; easing.type: Easing.OutCubic } }
                        }
                    }

                    // Low / High labels
                    Item {
                        width: parent.width
                        height: 16

                        Text {
                            anchors.left: parent.left
                            text: "Low"
                            color: "#88aa88"
                            font.pointSize: 10
                        }
                        Text {
                            anchors.right: parent.right
                            text: "High"
                            color: "#88aa88"
                            font.pointSize: 10
                        }
                    }
                }
            }
            Item { width: 1; height: 20 }

            // ── Note card ─────────────────────────────────────
            Rectangle {
                width: parent.width - 32
                anchors.horizontalCenter: parent.horizontalCenter
                height: noteRow.implicitHeight + 32
                radius: 14
                color: "white"
                border.color: "#e0e8de"; border.width: 1

                RowLayout {
                    id: noteRow
                    anchors {
                        fill: parent
                        margins: 16
                    }
                    spacing: 14

                    // Green icon box
                    Rectangle {
                        width: 42; height: 42; radius: 10
                        color: "#4a7c59"
                        Layout.alignment: Qt.AlignTop

                        Text {
                            anchors.centerIn: parent
                            text: "✦"
                            color: "white"
                            font.pointSize: 14
                        }
                    }

                    Column {
                        Layout.fillWidth: true
                        spacing: 6

                        Text {
                            text: "Note:"
                            color: "#1a2e1a"
                            font.pointSize: 12
                            font.bold: true
                        }

                        Text {
                            text: noteText
                            color: "#556655"
                            font.pointSize: 11
                            wrapMode: Text.WordWrap
                            lineHeight: 1.5
                            width: parent.width
                        }
                    }
                }
            }
        }
    }

    // ── Bottom action bar ────────────────────────────────────
    Rectangle {
        id: bottomBar
        anchors { bottom: parent.bottom; left: parent.left; right: parent.right }
        height: 96
        color: "#f0f4ef"

        // Top divider
        Rectangle {
            anchors { top: parent.top; left: parent.left; right: parent.right }
            height: 1; color: "#e0e8de"
        }

        RowLayout {
            anchors {
                fill: parent
                margins: 20
                bottomMargin: 16
            }
            spacing: 14

            // Re-generate
            // Re-generate
            Rectangle {
                Layout.fillWidth: true; height: 52
                radius: 26
                color: "white"
                border.color: "#c8d4c6"; border.width: 1.5

                Text {
                    anchors.centerIn: parent
                    text: "Re-generate"
                    color: "#3a4e3a"
                    font.pointSize: 13
                    font.letterSpacing: 0.3
                }

                scale: regenArea.containsPress ? 0.97 : 1.0
                Behavior on scale { NumberAnimation { duration: 100 } }

                MouseArea {
                    id: regenArea
                    anchors.fill: parent
                    onClicked: {
                        // Pop both InferenceResultScreen and CameraScreen off the stack
                        // then push a fresh CameraScreen so camera restarts cleanly
                        mainLoader.item.mainStackView.pop()  // back to CameraScreen
                        mainLoader.item.mainStackView.pop()  // back to HomeScreen
                        mainLoader.item.mainStackView.push(
                            "qrc:/qt/qml/PlantDoctor/ui/features/home/screens/CameraScreen.qml"
                        )
                    }
                }
            }

            // Share
            Rectangle {
                Layout.fillWidth: true; height: 52
                radius: 26
                color: "#4a7c59"

                // Shine
                Rectangle {
                    anchors { top: parent.top; left: parent.left; right: parent.right }
                    height: parent.height / 2; radius: parent.radius
                    color: "#15ffffff"
                }

                Text {
                    anchors.centerIn: parent
                    text: "Share"
                    color: "white"
                    font.pointSize: 13
                    font.bold: true
                    font.letterSpacing: 0.3
                }

                scale: shareArea.containsPress ? 0.97 : 1.0
                Behavior on scale { NumberAnimation { duration: 100 } }

                MouseArea {
                    id: shareArea
                    anchors.fill: parent
                    onClicked: console.log("Share tapped")
                }
            }
        }
    }
}
