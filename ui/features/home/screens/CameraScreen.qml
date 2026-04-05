import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtMultimedia
import QtQuick.Dialogs

Page {
    property alias camera: camera
    id: cameraScreen

    background: Rectangle {
        color: "#edf2e0"
    }

    property string capturedImagePath: ""
    property bool showPreview: false

    MediaDevices {
        id: mediaDevices
    }

    Component.onDestruction: {
        camera.active = false
    }

    CaptureSession {
        id: captureSession
        camera: Camera {
            id: camera
            focusMode: Camera.FocusModeAuto
            onErrorChanged: {
                if (error !== Camera.NoError) {
                    console.log("Camera error:", errorString)
                }
            }
        }
        videoOutput: videoOutput
        imageCapture: ImageCapture {
            id: imageCapture
            fileFormat: ImageCapture.PNG
            quality: ImageCapture.NormalQuality
            onImageCaptured: function(requestId, preview) {
                console.log("Image preview captured")
                Helper.imageToDataUrl(preview)
                camera.stop()
                Helper.setIsCamera(false)
                mainStackView.push("ImagePreviewScreen.qml")
            }
        }
    }

    // CAMERA VIEW
    Item {
        anchors.fill: parent

        VideoOutput {
            id: videoOutput
            anchors.fill: parent
        }

        // Scan frame
        Item {
            id: scanFrame
            width: parent.width * 0.72
            height: width * 1.1
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            anchors.verticalCenterOffset: -20

            Repeater {
                model: [
                    { hAnchor: "left",  vAnchor: "top" },
                    { hAnchor: "right", vAnchor: "top" },
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
                        width: parent.width
                        height: 3
                        radius: 2
                        color: "#5dde7a"
                        anchors.top:    cfg.vAnchor === "top"    ? parent.top    : undefined
                        anchors.bottom: cfg.vAnchor === "bottom" ? parent.bottom : undefined
                        anchors.left:   cfg.hAnchor === "left"   ? parent.left   : undefined
                        anchors.right:  cfg.hAnchor === "right"  ? parent.right  : undefined
                    }
                    Rectangle {
                        width: 3
                        height: parent.height
                        radius: 2
                        color: "#5dde7a"
                        anchors.top:    cfg.vAnchor === "top"    ? parent.top    : undefined
                        anchors.bottom: cfg.vAnchor === "bottom" ? parent.bottom : undefined
                        anchors.left:   cfg.hAnchor === "left"   ? parent.left   : undefined
                        anchors.right:  cfg.hAnchor === "right"  ? parent.right  : undefined
                    }
                }
            }
        }

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: scanFrame.bottom
            anchors.topMargin: 18
            text: "Centre the Leaf within the frame"
            color: "#99ffffff"
            font.pointSize: 11
            font.letterSpacing: 0.5
        }

        // Bottom controls
        Item {
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            height: 90
            anchors.bottomMargin: 52

            // Gallery button
            Button {
                id: galleryBtn
                width: 52; height: 52
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left; anchors.leftMargin: 48
                icon.source: "qrc:/assets/home/gallery-96.png"
                icon.width: 25; icon.height: 25; icon.color: "white"
                background: Rectangle {
                    anchors.fill: parent
                    radius: width/2
                    color: "#33ffffff"
                    border.color: "#44ffffff"
                    border.width: 1
                }
                onClicked: {
                    fileDialog.open()
                }
            }

            // Capture button
            Item {
                width: 82; height: 82
                anchors.centerIn: parent

                Rectangle {
                    anchors.fill: parent
                    radius: width/2
                    color: "transparent"
                    border.color: "#ccffffff"
                    border.width: 3
                }

                Rectangle {
                    width: 64; height: 64
                    radius: width/2
                    anchors.centerIn: parent
                    color: camera.active ? "white" : "#88ffffff"
                    scale: shutterArea.containsPress ? 0.88 : 1.0
                    Behavior on scale {
                        NumberAnimation { duration: 100; easing.type: Easing.OutQuad }
                    }

                    MouseArea {
                        id: shutterArea
                        anchors.fill: parent
                        enabled: camera.active
                        onClicked: { if(imageCapture.readyForCapture) imageCapture.capture() }
                    }
                }
            }

        }
    }
    //--------dialgo section-----
    FileDialog {
        id: fileDialog
        title: "Select an Image"
        onAccepted: {
            console.log("Selected file:", fileDialog.selectedFile);
            var path = fileDialog.selectedFile
            Helper.loadImageFromContentUri(path)
            mainStackView.push("ImagePreviewScreen.qml")
        }

        onRejected: {
            console.log("File selection canceled.");
        }
    }
    Component.onCompleted: { camera.start() }
    Connections {
        target: Helper
        function onIsCameraChanged(iscamera) {
            if(iscamera) {
                camera.start()
            }
        }

    }
}