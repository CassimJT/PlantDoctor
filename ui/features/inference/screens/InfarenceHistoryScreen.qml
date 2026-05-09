import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import HistoryModel

Page {
    id: inferenceHistoryScreen
    topPadding: 30

    property int timeDuration: 500
    property int pauseDuration: 200

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
        color: "#f0f4ef"

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
                    onClicked: console.log("Share tapped")
                }
            }
        }
    }



}