import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtMultimedia

Page {
    id: cameraScreen

    background: Rectangle { color: "black" }

    // --- Camera + Capture Session Setup ---
    CaptureSession {
        id: captureSession
        camera: Camera {
            id: camera
            active: true  // Starts camera when screen loads
        }
        imageCapture: ImageCapture {
            id: imageCapture

            onImageSaved: function(requestId, path) {
                console.log("Image saved to:", path)
                // Navigate back and pass the image path
                mainLoader.item.mainStackView.pop()
                // Optionally emit a signal or set a property to use the path
            }

            onErrorOccurred: function(requestId, error, message) {
                console.error("Capture error:", message)
            }
        }
        videoOutput: videoOutput
    }

    // --- Live Camera Preview ---
    VideoOutput {
        id: videoOutput
        anchors.fill: parent
    }

    // --- Capture Button ---
    Rectangle {
        width: 70
        height: 70
        radius: 35
        color: "white"
        border.color: "#cccccc"
        border.width: 3
        anchors {
            horizontalCenter: parent.horizontalCenter
            bottom: parent.bottom
            bottomMargin: 40
        }

        Rectangle {
            width: 54
            height: 54
            radius: 27
            color: "white"
            border.color: "#aaaaaa"
            border.width: 2
            anchors.centerIn: parent
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                imageCapture.captureToFile("")  // "" = default save location
            }
        }
    }

    // --- Back Button ---
    RoundButton {
        text: "✕"
        anchors {
            top: parent.top
            left: parent.left
            margins: 16
        }
        onClicked: {
            camera.active = false  // Stop camera before popping
            mainLoader.item.mainStackView.pop()
        }
    }

    // Stop camera when leaving screen
    Component.onDestruction: {
        camera.active = false
    }
}
