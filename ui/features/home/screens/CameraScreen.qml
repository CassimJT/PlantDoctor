import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtMultimedia

Page {
    id: cameraScreen

    background: Rectangle { color: "#edf2e0" }

    property string capturedImagePath: ""
    property bool showPreview: false

    // Start camera after short delay to avoid buffer conflicts
    Component.onCompleted: startTimer.start()

    Timer {
        id: startTimer
        interval: 300
        repeat: false
        onTriggered: camera.active = true
    }

    // Restart camera after retake with delay to let buffers release
    Timer {
        id: retakeTimer
        interval: 400
        repeat: false
        onTriggered: camera.active = true
    }

    Component.onDestruction: { camera.active = false }

    CaptureSession {
        id: captureSession
        camera: Camera {
            id: camera
            active: false  // ← controlled by timers, not directly
        }
        imageCapture: ImageCapture {
            id: imageCapture
            onImageSaved: function(requestId, path) {
                capturedImagePath = path
                showPreview = true
                previewReveal.start()
            }
            onErrorOccurred: function(requestId, error, message) {
                console.error("Capture error:", message)
            }
        }
        videoOutput: videoOutput
    }


    // CAMERA VIEW

    Item {
        anchors.fill: parent
        visible: !showPreview
        anchors.topMargin: 20

        VideoOutput {
            id: videoOutput
            anchors.fill: parent
        }

        // Scan frame
        Item {
            id: scanFrame
            width: parent.width * 0.72
            height: width * 1.1
            anchors.centerIn: parent
            anchors.verticalCenterOffset: -20

            Repeater {
                model: [
                    { hAnchor: "left",  vAnchor: "top"    },
                    { hAnchor: "right", vAnchor: "top"    },
                    { hAnchor: "left",  vAnchor: "bottom" },
                    { hAnchor: "right", vAnchor: "bottom" }
                ]
                Item {
                    property var cfg: modelData
                    anchors.left:   cfg.hAnchor === "left"   ? parent.left   : undefined
                    anchors.right:  cfg.hAnchor === "right"  ? parent.right  : undefined
                    anchors.top:    cfg.vAnchor === "top"    ? parent.top    : undefined
                    anchors.bottom: cfg.vAnchor === "bottom" ? parent.bottom : undefined
                    width: 36; height: 36

                    Rectangle {
                        width: parent.width; height: 3; radius: 2; color: "#5dde7a"
                        anchors.top:    cfg.vAnchor === "top"    ? parent.top    : undefined
                        anchors.bottom: cfg.vAnchor === "bottom" ? parent.bottom : undefined
                        anchors.left:   cfg.hAnchor === "left"   ? parent.left   : undefined
                        anchors.right:  cfg.hAnchor === "right"  ? parent.right  : undefined
                    }
                    Rectangle {
                        width: 3; height: parent.height; radius: 2; color: "#5dde7a"
                        anchors.top:    cfg.vAnchor === "top"    ? parent.top    : undefined
                        anchors.bottom: cfg.vAnchor === "bottom" ? parent.bottom : undefined
                        anchors.left:   cfg.hAnchor === "left"   ? parent.left   : undefined
                        anchors.right:  cfg.hAnchor === "right"  ? parent.right  : undefined
                    }
                }
            }
        }

        Text {
            anchors {
                horizontalCenter: parent.horizontalCenter
                top: scanFrame.bottom; topMargin: 18
            }
            text: "Centre the Leaf within the frame"
            color: "#99ffffff"; font.pointSize: 11; font.letterSpacing: 0.5
        }

        // Bottom controls
        Item {
            anchors {
                bottom: parent.bottom; left: parent.left
                right: parent.right; bottomMargin: 52
            }
            height: 90

            Button {
                id: galleryBtn
                width: 52; height: 52
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left; anchors.leftMargin: 48
                icon.source: "qrc:/assets/home/gallery-96.png"
                icon.width: 25; icon.height: 25; icon.color: "white"
                background: Rectangle {
                    anchors.fill: parent
                    radius: width / 2
                    color: "#33ffffff"; border.color: "#44ffffff"; border.width: 1
                }
            }

            // Capture button
            Item {
                width: 82; height: 82
                anchors.centerIn: parent

                Rectangle {
                    anchors.fill: parent; radius: width / 2
                    color: "transparent"; border.color: "#ccffffff"; border.width: 3
                }
                Rectangle {
                    width: 64; height: 64; radius: width / 2
                    anchors.centerIn: parent
                    color: camera.active ? "white" : "#88ffffff"  // dims when not ready
                    scale: shutterArea.containsPress ? 0.88 : 1.0
                    Behavior on scale { NumberAnimation { duration: 100; easing.type: Easing.OutQuad } }
                    MouseArea {
                        id: shutterArea
                        anchors.fill: parent
                        enabled: camera.active  // blocks tap until camera ready
                        onClicked: imageCapture.captureToFile("")
                    }
                }
            }

            Button {
                id: swithingCamera
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right; anchors.rightMargin: 48
                width: 52; height: 52
                icon.source: "qrc:/assets/home/icons8-switch-camera-100.png"
                icon.width: 25; icon.height: 25; icon.color: "white"
                background: Rectangle {
                    anchors.fill: parent
                    radius: width / 2
                    color: "#33ffffff"; border.color: "#44ffffff"; border.width: 1
                }
            }
        }
    }


    // PREVIEW VIEW

    Item {
        id: previewView
        anchors.fill: parent
        visible: showPreview
        opacity: 0

        PropertyAnimation {
            id: previewReveal
            target: previewView; property: "opacity"
            from: 0; to: 1; duration: 350; easing.type: Easing.OutCubic
        }

        Rectangle {
            anchors.fill: parent
            color: "#edf2e0"
        }

        // Top bar
        RowLayout {
            id: previewTopBar
            anchors {
                top: parent.top; left: parent.left; right: parent.right
                margins: 20; topMargin: 52
            }

            Item { Layout.fillWidth: true }

            Rectangle {
                height: 32; width: previewLabel.implicitWidth + 24
                radius: 16; color: "#33ffffff"; border.color: "#44ffffff"; border.width: 1
                Text {
                    id: previewLabel
                    anchors.centerIn: parent
                    text: "Preview";
                    font.pointSize: 11; font.letterSpacing: 1.2
                }
            }

            Item { Layout.fillWidth: true }
            Item { width: 42; height: 42 }
        }

        // Photo frame
        Item {
            id: photoFrame
            width: parent.width - 48
            height: parent.height * 0.58
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: previewTopBar.bottom
            anchors.topMargin: 20

            Rectangle {
                anchors.centerIn: parent
                width: parent.width + 12
                height: parent.height + 12
                radius: 22
                color: "transparent"
                border.color: "#335dde7a"
                border.width: 3
            }

            Rectangle {
                anchors.fill: parent
                radius: 18
                color: "#1a1f1a"
                border.color: "#22ffffff"
                border.width: 1

                Rectangle {
                    id: photoClip
                    anchors {
                        top: parent.top; left: parent.left
                        right: parent.right; bottom: capturedLabel.top
                        margins: 10; bottomMargin: 0
                    }
                    radius: 12; clip: true; color: "transparent"

                    Image {
                        anchors.fill: parent
                        source: capturedImagePath ? "file://" + capturedImagePath : ""
                        fillMode: Image.PreserveAspectCrop
                    }

                    Rectangle {
                        anchors.fill: parent; radius: 12
                        color: "transparent"
                        border.color: "#44000000"; border.width: 1
                    }
                }

                Item {
                    id: capturedLabel
                    anchors {
                        bottom: parent.bottom; left: parent.left
                        right: parent.right; margins: 10; bottomMargin: 10
                    }
                    height: 28
                }
            }
        }

        Text {
            anchors {
                horizontalCenter: parent.horizontalCenter
                top: photoFrame.bottom; topMargin: 14
            }
            text: "Looking good? Tap Analyse to identify"
            color: "#66ffffff"; font.pointSize: 11; font.letterSpacing: 0.4
        }

        // Action buttons

                Column {
                    anchors {
                        bottom: parent.bottom
                        left: parent.left; right: parent.right
                        margins: 24; bottomMargin: 48
                    }
                    spacing: 12

                    // Analyse
                    Rectangle {
                        width: parent.width
                        height: 60
                        radius: 30
                        gradient: Gradient {
                            orientation: Gradient.Horizontal
                            GradientStop { position: 0.0; color: analyseArea.containsPress ? "#2a9e48" : "#34c45a" }
                            GradientStop { position: 1.0; color: analyseArea.containsPress ? "#3dbf60" : "#5dde7a" }
                        }

                        // Top gloss shine
                        Rectangle {
                            anchors { top: parent.top; left: parent.left; right: parent.right }
                            height: parent.height / 2
                            radius: parent.radius
                            color: "#1affffff"
                        }

                        // Outer glow ring
                        Rectangle {
                            anchors.fill: parent
                            radius: parent.radius
                            color: "transparent"
                            border.color: "#445dde7a"
                            border.width: 1
                        }

                        Row {
                            anchors.centerIn: parent
                            spacing: 10

                            Image {
                                source: "qrc:/assets/home/icons8-analyse-60.png"
                                width: 24; height: 24
                                anchors.verticalCenter: parent.verticalCenter
                            }
                            Text {
                                text: "Analyse Plant"
                                color: "white"
                                font.pointSize: 16
                                font.bold: true
                                font.letterSpacing: 0.6
                                anchors.verticalCenter: parent.verticalCenter
                            }
                        }

                        scale: analyseArea.containsPress ? 0.97 : 1.0
                        Behavior on scale { NumberAnimation { duration: 100; easing.type: Easing.OutQuad } }

                        MouseArea {
                            id: analyseArea
                            anchors.fill: parent
                            onClicked: {
                                mainLoader.item.mainStackView.push(
                                    "qrc:/qt/qml/PlantDoctor/ui/features/inference/screens/InfarenceResultScreen.qml",
                                    { "imagePath": capturedImagePath }
                                )
                            }
                        }
                    }

                    // Retake
                    Rectangle {
                        width: parent.width
                        height: 52
                        radius: 26
                        color: "#02a3b5"
                        border.color: "#44ffffff"
                        border.width: 1.5

                        Row {
                            anchors.centerIn: parent
                            spacing: 8

                            Image {
                                source: "qrc:/assets/home/icons8-retake-100.png"
                                width: 20; height: 20
                                anchors.verticalCenter: parent.verticalCenter
                            }
                            Text {
                                text: "Retake Photo"
                                color: "#ccffffff"
                                font.pointSize: 16
                                font.letterSpacing: 0.4
                                anchors.verticalCenter: parent.verticalCenter
                            }
                        }

                        // Press fill
                        scale: retakeArea.containsPress ? 0.97 : 1.0
                        Behavior on scale { NumberAnimation { duration: 100; easing.type: Easing.OutQuad } }

                        MouseArea {
                            id: retakeArea
                            anchors.fill: parent
                            onClicked: {
                                capturedImagePath = ""
                                showPreview = false
                                retakeTimer.start()
                            }
                        }
                    }
                }
    }
}
