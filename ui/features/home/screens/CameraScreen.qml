import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtMultimedia

Page {
    id: cameraScreen

    background: Rectangle { color: "#0a0f0a" }

    property string capturedImagePath: ""
    property bool showPreview: false

    CaptureSession {
        id: captureSession
        camera: Camera {
            id: camera
            active: !showPreview
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

    // =========================================================
    // CAMERA VIEW
    // =========================================================
    Item {
        anchors.fill: parent
        visible: !showPreview

        VideoOutput {
            id: videoOutput
            anchors.fill: parent
        }

        // Top vignette
        Rectangle {
            anchors { top: parent.top; left: parent.left; right: parent.right }
            height: 160
            gradient: Gradient {
                GradientStop { position: 0.0; color: "#cc0a0f0a" }
                GradientStop { position: 1.0; color: "#000a0f0a" }
            }
        }

        // Bottom vignette
        Rectangle {
            anchors { bottom: parent.bottom; left: parent.left; right: parent.right }
            height: 220
            gradient: Gradient {
                GradientStop { position: 0.0; color: "#000a0f0a" }
                GradientStop { position: 1.0; color: "#ee0a0f0a" }
            }
        }

        // Top bar
        RowLayout {
            anchors {
                top: parent.top; left: parent.left; right: parent.right
                margins: 20; topMargin: 52
            }

            Rectangle {
                width: 42; height: 42; radius: 21
                color: "#33ffffff"; border.color: "#55ffffff"; border.width: 1
                Text {
                    anchors.centerIn: parent
                    text: "X"; color: "white"; font.pointSize: 15; font.bold: true
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        camera.active = false
                        mainLoader.item.mainStackView.pop()
                    }
                }
            }

            Item { Layout.fillWidth: true }

            Rectangle {
                height: 32; width: chipLabel.implicitWidth + 24
                radius: 16; color: "#33ffffff"; border.color: "#44ffffff"; border.width: 1
                Text {
                    id: chipLabel
                    anchors.centerIn: parent
                    text: "Plant Scanner"; color: "white"
                    font.pointSize: 11; font.letterSpacing: 1.2
                }
            }

            Item { Layout.fillWidth: true }

            Rectangle {
                width: 42; height: 42; radius: 21
                color: "#33ffffff"; border.color: "#55ffffff"; border.width: 1
                Text { anchors.centerIn: parent; text: "⚡"; font.pointSize: 15 }
            }
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

            Rectangle {
                id: scanLine
                width: parent.width - 4; height: 2; radius: 1
                anchors.horizontalCenter: parent.horizontalCenter
                y: 4
                gradient: Gradient {
                    orientation: Gradient.Horizontal
                    GradientStop { position: 0.0; color: "transparent" }
                    GradientStop { position: 0.3; color: "#885dde7a" }
                    GradientStop { position: 0.5; color: "#ff5dde7a" }
                    GradientStop { position: 0.7; color: "#885dde7a" }
                    GradientStop { position: 1.0; color: "transparent" }
                }
                SequentialAnimation {
                    running: true; loops: Animation.Infinite
                    NumberAnimation {
                        target: scanLine; property: "y"
                        from: 4; to: scanFrame.height - 4
                        duration: 2200; easing.type: Easing.InOutSine
                    }
                    NumberAnimation {
                        target: scanLine; property: "y"
                        from: scanFrame.height - 4; to: 4
                        duration: 2200; easing.type: Easing.InOutSine
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
            Button{
                id: galleryBtn
                width: 52; height: 52;
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left; anchors.leftMargin: 48
                icon.source: "qrc:/assets/home/gallery-96.png"
                icon.width: 25
                icon.height: 25
                icon.color: "white"
                background:Rectangle {
                    anchors.fill: parent
                    radius: width / 2
                    color: "#33ffffff"; border.color: "#44ffffff"; border.width: 1
                }
            }


                //capture photos button
            Item {
                width: 82; height: 82
                anchors.centerIn: parent

                Rectangle {
                    anchors.fill: parent; radius: width / 2
                    color: "transparent"; border.color: "#ccffffff"; border.width: 3
                }
                Rectangle {
                    width: 64; height: 64; radius: width / 2
                    anchors.centerIn: parent; color: "white"
                    scale: shutterArea.containsPress ? 0.88 : 1.0
                    Behavior on scale { NumberAnimation { duration: 100; easing.type: Easing.OutQuad } }
                    MouseArea {
                        id: shutterArea
                        anchors.fill: parent
                        onClicked: imageCapture.captureToFile("")
                    }
                }
            }

            Button {
                id: swithingCamera
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right; anchors.rightMargin: 48
                width: 52; height: 52;
                icon.source: "qrc:/assets/home/icons8-switch-camera-100.png"
                icon.width: 25
                icon.height: 25
                icon.color: "white"
                //will add icon ere
                background: Rectangle {
                    anchors.fill: parent
                    radius: width / 2
                    color: "#33ffffff"; border.color: "#44ffffff"; border.width: 1

                }
            }
        }
    }

    // =========================================================
    // PREVIEW VIEW
    // =========================================================
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

        Image {
            anchors.fill: parent
            source: capturedImagePath ? "file://" + capturedImagePath : ""
            fillMode: Image.PreserveAspectCrop
        }

        // Top vignette
        Rectangle {
            anchors { top: parent.top; left: parent.left; right: parent.right }
            height: 140
            gradient: Gradient {
                GradientStop { position: 0.0; color: "#bb0a0f0a" }
                GradientStop { position: 1.0; color: "#000a0f0a" }
            }
        }

        // Bottom vignette
        Rectangle {
            anchors { bottom: parent.bottom; left: parent.left; right: parent.right }
            height: 200
            gradient: Gradient {
                GradientStop { position: 0.0; color: "#000a0f0a" }
                GradientStop { position: 1.0; color: "#cc0a0f0a" }
            }
        }

        // Top bar
        RowLayout {
            anchors {
                top: parent.top; left: parent.left; right: parent.right
                margins: 20; topMargin: 52
            }

            Rectangle {
                width: 42; height: 42; radius: 21
                color: "#33ffffff"; border.color: "#55ffffff"; border.width: 1
                Text {
                    anchors.centerIn: parent
                    text: "✕"; color: "white"; font.pointSize: 15; font.bold: true
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        camera.active = false
                        mainLoader.item.mainStackView.pop()
                    }
                }
            }

            Item { Layout.fillWidth: true }

            Rectangle {
                height: 32; width: previewLabel.implicitWidth + 24
                radius: 16; color: "#33ffffff"; border.color: "#44ffffff"; border.width: 1
                Text {
                    id: previewLabel
                    anchors.centerIn: parent
                    text: "Preview"; color: "white"
                    font.pointSize: 11; font.letterSpacing: 1.2
                }
            }

            Item { Layout.fillWidth: true }
            Item { width: 42; height: 42 }
        }

        // Action buttons
        RowLayout {
            anchors {
                bottom: parent.bottom; left: parent.left; right: parent.right
                margins: 28; bottomMargin: 56
            }
            spacing: 16

            // Retake
            Rectangle {
                Layout.fillWidth: true; height: 56; radius: 28
                color: "#33ffffff"; border.color: "#88ffffff"; border.width: 1

                RowLayout {
                    anchors.centerIn: parent; spacing: 8
                    Text { text: "↺"; color: "white"; font.pointSize: 17; font.bold: true }
                    Text { text: "Retake"; color: "white"; font.pointSize: 14; font.letterSpacing: 0.5 }
                }

                scale: retakeArea.containsPress ? 0.96 : 1.0
                Behavior on scale { NumberAnimation { duration: 100 } }

                MouseArea {
                    id: retakeArea
                    anchors.fill: parent
                    onClicked: {
                        capturedImagePath = ""
                        showPreview = false
                    }
                }
            }

            // Analyse button
            Rectangle {
                Layout.fillWidth: true; height: 56; radius: 28
                gradient: Gradient {
                    orientation: Gradient.Horizontal
                    GradientStop { position: 0.0; color: "#3dba57" }
                    GradientStop { position: 1.0; color: "#5dde7a" }
                }

                Rectangle {
                    anchors.fill: parent; radius: parent.radius
                    color: "transparent"; border.color: "#885dde7a"; border.width: 1
                }

                RowLayout {
                    anchors.centerIn: parent; spacing: 8
                    Text { text: "🔍"; font.pointSize: 15 }
                    Text {
                        text: "Analyse"; color: "white"
                        font.pointSize: 14; font.bold: true; font.letterSpacing: 0.5
                    }
                }

                scale: analyseArea.containsPress ? 0.96 : 1.0
                Behavior on scale { NumberAnimation { duration: 100 } }

                MouseArea {
                    id: analyseArea
                    anchors.fill: parent
                    onClicked: {
                       console.log("Photo being analysed...go to results page")
                    }
                }
            }
        }
    }

    Component.onDestruction: { camera.active = false }
}

