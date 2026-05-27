import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import HistoryModel
import "../components"

Page {
    id: inferenceResultScreen
    topPadding: 30

    property int timeDuration: 250
    property int pauseDuration: 150

    property string imagePath: ""
    property string plantName: InfarenceRunner.diseaseName
    property string diseaseName: InfarenceRunner.diseaseName
    property real riskLevel: InfarenceRunner.riskLevel
    property string description: InfarenceRunner.description
    property real confidence: InfarenceRunner.confidence
    property var treatments: InfarenceRunner.cure ? InfarenceRunner.cure.split("\n") : []
    property string noteText: ""

    background: Rectangle { color: "#edf2e0" }

    opacity: 0
    Component.onCompleted: {
        fadeIn.start()
        fadeSequence.start()

        // Check confidence after the screen loads
        checkConfidenceAndShowSurvey()
    }

    // Function to check confidence and show survey
    function checkConfidenceAndShowSurvey() {
        // Wait a moment for the animations to complete
        surveyTimer.start()
    }
    SurveyPopUp {
        id: surveyPopUp
    }

    Timer {
        id: surveyTimer
        interval: 1000  // Wait 1 second after screen loads
        repeat: false
        onTriggered: {
            if (confidence < 50) {
                console.log("Confidence is low: " + confidence + "%. Opening survey...")
                surveyPopUp.open()
            } else {
                console.log("Confidence is good: " + confidence + "%. No survey needed.")
            }
        }
    }

    PropertyAnimation {
        id: fadeIn
        target: inferenceResultScreen
        property: "opacity"
        from: 0
        to: 1
        duration: 380
    }

    ScrollView {
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            bottom: bottomBar.top
        }
        contentWidth: availableWidth
        clip: true

        Column {
            width: availableWidth
            spacing: 10
            topPadding: 60
            bottomPadding: 120

            // ── Hero Card with Rounded Image ──
            Rectangle {
                width: parent.width - 32
                height: 100
                anchors.horizontalCenter: parent.horizontalCenter
                radius: 16
                color: "#1e2b1e"

                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 14
                    spacing: 14

                    Rectangle {
                        width: 75
                        height: 75
                        radius: 20
                        color: "#2d3d2d"

                        Image {
                            id: thumbLoader
                            visible: false
                            source: imagePath
                            fillMode: Image.PreserveAspectCrop
                            onStatusChanged: {
                                if (status === Image.Ready) {
                                    thumbCanvas.requestPaint()
                                }
                            }
                        }

                        Canvas {
                            id: thumbCanvas
                            anchors.fill: parent
                            onPaint: {
                                var ctx = getContext("2d")
                                ctx.clearRect(0, 0, width, height)
                                var radius = 20

                                ctx.beginPath()
                                ctx.moveTo(radius, 0)
                                ctx.lineTo(width - radius, 0)
                                ctx.quadraticCurveTo(width, 0, width, radius)
                                ctx.lineTo(width, height - radius)
                                ctx.quadraticCurveTo(width, height, width - radius, height)
                                ctx.lineTo(radius, height)
                                ctx.quadraticCurveTo(0, height, 0, height - radius)
                                ctx.lineTo(0, radius)
                                ctx.quadraticCurveTo(0, 0, radius, 0)
                                ctx.closePath()
                                ctx.clip()

                                if (thumbLoader.status === Image.Ready) {
                                    ctx.drawImage(thumbLoader, 0, 0, width, height)
                                }
                            }
                        }

                        Text {
                            anchors.centerIn: parent
                            text: ""
                            font.pointSize: 28
                            visible: imagePath === ""
                        }
                    }

                    Column {
                        Layout.fillWidth: true
                        spacing: 4

                        Text {
                            id: titleText
                            text: diseaseName.split(" ")[0]
                            color: "white"
                            font.bold: true
                            opacity: 0
                            Behavior on opacity { NumberAnimation { duration: timeDuration } }
                        }

                        Text {
                            id: plantNameText
                            text: plantName
                            color: "#88aa88"
                            opacity: 0
                            Behavior on opacity { NumberAnimation { duration: timeDuration } }
                        }
                    }
                }
            }

            Item { height: 20 }

            // ── Headings & Description ──
            Text {
                id: diseaseHeading
                text: diseaseName + " on plant"
                anchors.left: parent.left
                anchors.leftMargin: 20
                font.bold: true
                opacity: 0
            }

            Item { height: 10 }

            Text {
                id: descriptionText
                text: description
                width: availableWidth - 40
                anchors.left: parent.left
                anchors.leftMargin: 20
                wrapMode: Text.WordWrap
                opacity: 0
                Behavior on opacity { NumberAnimation { duration: timeDuration } }
            }

            Item { height: 22 }

            Text {
                id: treatmentHeading
                text: "Treatment and Prevention"
                anchors.left: parent.left
                anchors.leftMargin: 20
                font.bold: true
                opacity: 0
            }

            Item { height: 12 }

            // ── Treatment Items ──
            Repeater {
                id: treatmentRepeater
                model: treatments

                Text {
                    width: availableWidth - 40
                    anchors.left: parent.left
                    anchors.leftMargin: 20
                    wrapMode: Text.WordWrap
                    text: "• " + modelData
                    opacity: 0
                    Behavior on opacity { NumberAnimation { duration: timeDuration } }
                }
            }

            Item { height: 20 }

            // ── Risk Card ──
            Rectangle {
                id: riskCard
                width: parent.width - 32
                anchors.horizontalCenter: parent.horizontalCenter
                height: 90
                radius: 14
                color: "white"
                border.color: "#e0e8de"
                border.width: 1
                opacity: 0
                Behavior on opacity { NumberAnimation { duration: timeDuration } }

                Column {
                    anchors.fill: parent
                    anchors.margins: 16
                    spacing: 10

                    Text {
                        id: riskHeading
                        text: "Risk life prediction"
                        font.bold: true
                        opacity: 0
                        Behavior on opacity { NumberAnimation { duration: timeDuration } }
                    }

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

                    Item { height: 16 }

                    RowLayout {
                        width: parent.width
                        spacing: 0

                        Text {
                            text: "Low"
                            color: "#88aa88"
                            font.pointSize: 10
                            anchors.left: parent.left
                        }

                        Text {
                            text: "High"
                            color: "#88aa88"
                            font.pointSize: 10
                            anchors.right: parent.right
                        }
                    }
                }
            }

            Item { height: 40 }

            // ── Floating Confidence Button ──
            RowLayout {
                id: confidenceRow
                anchors.right: parent.right
                anchors.rightMargin: 25
                spacing: 6

                Label {
                    id: confiLabel
                    text: "confidence"
                    opacity: 0
                    Layout.alignment: Qt.AlignHCenter
                    Behavior on opacity { NumberAnimation { duration: timeDuration } }
                }

                RoundButton {
                    id: roundBtn
                    Layout.preferredWidth:  70
                    Layout.preferredHeight:  70
                    opacity: 0
                    Behavior on opacity { NumberAnimation { duration: timeDuration } }

                    Label {
                        anchors.centerIn: parent
                        text: confidence.toFixed(1) + "%"
                        color: confidence < 50 ? "red" : "green"
                        font.bold: true
                    }
                }

            }
        }
    }

    // ── Bottom Bar ──
    Rectangle {
        id: bottomBar
        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }
        height: 80
        color: "#edf2e0"

        Rectangle {
            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
            }
            height: 1
            color: "#e0e8de"
        }

        RowLayout {
            anchors {
                fill: parent
                margins: 20
                bottomMargin: 16
            }
            spacing: 14

            Rectangle {
                Layout.fillWidth: true
                height: 52
                radius: 26
                color: "white"
                border.color: "#c8d4c6"
                border.width: 1.5

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
                        mainLoader.item.mainStackView.pop()
                        mainLoader.item.mainStackView.pop()
                    }
                }
            }

            Rectangle {
                Layout.fillWidth: true
                height: 52
                radius: 26
                color: "#4a7c59"

                Rectangle {
                    anchors {
                        top: parent.top
                        left: parent.left
                        right: parent.right
                    }
                    height: parent.height / 2
                    radius: parent.radius
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

                }
            }
        }
    }

    // SURVEY DRAWER
    Drawer {
        id: surveyDrawer

        // Slide up from the bottom of the screen
        edge: Qt.BottomEdge
        width: parent.width
        // Set a height that perfectly fits our form content comfortably
        height: parent.height * 0.40

        // Property to receive the crop name dynamically from your backend/screen
        property string currentCropName: plantName || "crop"

        // Signals to pass data back to your backend processing functions
        signal surveySubmitted(string variety, string imageSource)
        signal surveySkipped()

        background: Rectangle {
            color: "white"
            radius: 24
            // Clip to ensure content doesn't bleed past the rounded top corners
            layer.enabled: true

            // Visual indicator/handle at the top of the sheet showing it can be dragged down
            Rectangle {
                anchors.horizontalCenter: parent.horizontalCenter
                y: 8
                width: 40
                height: 4
                radius: 2
                color: "#d4ddd0"
            }
        }

        contentItem: ColumnLayout {
            anchors {
                fill: parent
                margins: 24
                topMargin: 20 // Extra room for the drag handle
            }
            spacing: 12

            // ── Header Text ───────────────────────────────────────
            ColumnLayout {
                Layout.fillWidth: true
                spacing: 4

                Text {
                    text: "Help us improve!"
                    color: "#1a2e1a"
                    font.pointSize: 16
                    font.bold: true
                }

                Text {
                    // Dynamically injecting the crop name from backend string
                    text: "Please answer 2 quick questions about your " + surveyDrawer.currentCropName + " diagnosis."
                    color: "#778877"
                    font.pointSize: 11
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                }
            }

            // ── Input 1: Crop Variety ─────────────────────────────
            ColumnLayout {
                Layout.fillWidth: true
                spacing: 6

                Text {
                    text: "What variety of " + surveyDrawer.currentCropName + " did you plant?"
                    color: "#334433"
                    font.pointSize: 13
                    font.bold: true
                }

                TextField {
                    id: varietyInput
                    Layout.fillWidth: true
                    placeholderText: "e.g. Local, Hybrid, kanyani..."
                    selectByMouse: true

                    background: Rectangle {
                        implicitHeight: 46
                        radius: 12
                        color: varietyInput.activeFocus ? "#edf5e8" : "#f7faf6"
                        border.color: varietyInput.activeFocus ? "#34c45a" : "#d4ddd0"
                        border.width: varietyInput.activeFocus ? 2 : 1
                    }
                }
            }

            // ── Input 2: Image Source Dropdown ────────────────────
            ColumnLayout {
                Layout.fillWidth: true
                spacing: 6

                Text {
                    text: "Where did you get the diagnosis image?"
                    color: "#334433"
                    font.pointSize: 13
                    font.bold: true
                }

                ComboBox {
                    id: sourceDropdown
                    Layout.fillWidth: true
                    model: ["Captured from the field", "From the internet"]

                    delegate: ItemDelegate {
                        width: sourceDropdown.width
                        contentItem: Text {
                            text: modelData
                            color: "#334433"
                            font.pointSize: 12
                            verticalAlignment: Text.AlignVCenter
                        }
                        highlighted: sourceDropdown.highlightedIndex === index
                    }

                    background: Rectangle {
                        implicitHeight: 46
                        radius: 12
                        color: "#f7faf6"
                        border.color: "#d4ddd0"
                        border.width: 1
                    }
                }
            }

            // ── Action Buttons ────────────────────────────────────
            RowLayout {
                Layout.fillWidth: true
                spacing: 12

                // Not Now / Skip Button
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 48
                    radius: 24
                    color: skipArea.containsPress ? "#e0e8da" : "#edf2e0"
                    border.color: "#c8d4c0"

                    Text {
                        anchors.centerIn: parent
                        text: "Not now"
                        color: "#556655"
                        font.pointSize: 13
                        font.bold: true
                    }

                    MouseArea {
                        id: skipArea
                        anchors.fill: parent
                        onClicked: {
                            surveyDrawer.surveySkipped()
                            surveyDrawer.close()
                        }
                    }
                }

                // Submit Button
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 48
                    radius: 24
                    gradient: Gradient {
                        orientation: Gradient.Horizontal
                        GradientStop { position: 0.0; color: submitArea.containsPress ? "#2a9e48" : "#34c45a" }
                        GradientStop { position: 1.0; color: submitArea.containsPress ? "#3dbf60" : "#5dde7a" }
                    }

                    Text {
                        anchors.centerIn: parent
                        text: "Submit"
                        color: "white"
                        font.pointSize: 13
                        font.bold: true
                    }

                    MouseArea {
                        id: submitArea
                        anchors.fill: parent
                        onClicked: {
                            // Pass parameters directly into the signal handler execution
                            surveyDrawer.surveySubmitted(varietyInput.text, sourceDropdown.currentText)
                            surveyDrawer.close()
                        }
                    }
                }
            }
        }
    }

    // ── Sequential Fade-In Animations ──
    SequentialAnimation {
        id: fadeSequence

        PropertyAnimation { target: titleText; property: "opacity"; to: 1; duration: timeDuration }
        PauseAnimation { duration: pauseDuration }

        PropertyAnimation { target: plantNameText; property: "opacity"; to: 1; duration: timeDuration }
        PauseAnimation { duration: pauseDuration }

        PropertyAnimation { target: diseaseHeading; property: "opacity"; to: 1; duration: timeDuration }
        PauseAnimation { duration: pauseDuration }

        PropertyAnimation { target: descriptionText; property: "opacity"; to: 1; duration: timeDuration }
        PauseAnimation { duration: pauseDuration }

        PropertyAnimation { target: treatmentHeading; property: "opacity"; to: 1; duration: timeDuration }
        PauseAnimation { duration: pauseDuration }

        ScriptAction {
            script: {
                for (let i = 0; i < treatmentRepeater.count; i++) {
                    let item = treatmentRepeater.itemAt(i)
                    if (item) item.opacity = 1
                }
            }
        }

        PauseAnimation { duration: pauseDuration }

        PropertyAnimation { target: riskHeading; property: "opacity"; to: 1; duration: timeDuration }
        PauseAnimation { duration: pauseDuration }

        PropertyAnimation { target: riskCard; property: "opacity"; to: 1; duration: timeDuration }
        PauseAnimation { duration: pauseDuration }

        PropertyAnimation { target: roundBtn; property: "opacity"; to: 1; duration: timeDuration }
        PropertyAnimation { target: confiLabel; property: "opacity"; to: 1; duration: timeDuration }
    }
}