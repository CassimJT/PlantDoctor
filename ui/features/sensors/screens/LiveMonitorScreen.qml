import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import RTSVideoOutput

Page {
    id: livemonitor

    objectName: "Live Pest Monitor"
    topPadding: 70

    // Background gradient
    Rectangle {
        anchors.fill: parent
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#1a1a1a" }
            GradientStop { position: 1.0; color: "#0d0d0d" }
        }
    }

    // Main video output
    RTSVideoOutput {
        id: videoOutput
        anchors {
            fill: parent
            margins: Qt.platform.os === "android" ? 0 : 10
        }
        rtsUrl: "http://192.168.8.117/mjpeg"
        detectionEnabled: false

        // Fixed: Use proper parameter syntax
        onDetectionResult: function(result) {
            console.log("Detection:", result)
            notificationPopup.text = result
            notificationPopup.visible = true
            notificationPopupTimer.start()
        }

        onIsConnectedChanged: function(isConnected) {
            if (isConnected) {
                console.log("Camera connected")
                connectionStatusText.text = "Camera CONNECTED"
                connectionStatusText.color = "#4caf50"
                reconnectButton.visible = false
                enableDetectionButton.enabled = true
                enableDetectionButton.opacity = 1.0
            } else {
                console.log("Camera disconnected")
                videoOutput.detectionEnabled = false
                detectionStatusText.text = "Detection: DISABLED"
                detectionStatusText.color = "#f44336"
                connectionStatusText.text = "Camera OFFLINE - Tap to reconnect"
                connectionStatusText.color = "#f44336"
                reconnectButton.visible = true
                enableDetectionButton.enabled = false
                enableDetectionButton.opacity = 0.5
            }
        }

        onModelloaded: {
            console.log("Pest detection model loaded successfully")
            modelStatusText.text = "Model: LOADED ✓"
            modelStatusText.color = "#4caf50"
            enableDetectionButton.enabled = videoOutput.isConnected
            enableDetectionButton.opacity = videoOutput.isConnected ? 1.0 : 0.5
        }

        // Fixed: Add error parameter
        onModelLoadingFailed: function(error) {
            console.log("Model loading failed:", error)
            modelStatusText.text = "Model: FAILED ✗"
            modelStatusText.color = "#f44336"
        }

        // Component.onCompleted: Load model explicitly
        Component.onCompleted: {
            console.log("RTSVideoOutput component completed, loading model...")
            // The model should load automatically, but we can ensure it's loaded
            if (typeof videoOutput.modelloaded !== 'undefined') {
                console.log("Model loading will happen automatically")
            }
        }
    }

    // Semi-transparent overlay for status info
    Rectangle {
        id: infoOverlay
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            margins: Qt.platform.os === "android" ? 15 : 10
        }
        height: statusColumn.height + 20
        color: "#80000000"
        radius: 8

        Column {
            id: statusColumn
            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
                margins: 10
            }
            spacing: 5

            Text {
                text: "🌿 Pest Detection System"
                color: "#ffffff"
                font.pixelSize: Qt.platform.os === "android" ? 16 : 14
                font.bold: true
                font.family: "sans-serif"
            }

            Rectangle {
                width: parent.width
                height: 1
                color: "#4caf50"
                opacity: 0.5
            }

            Row {
                spacing: 10
                Rectangle {
                    width: 8
                    height: 8
                    radius: 4
                    color: videoOutput.detectionEnabled ? "#4caf50" : "#f44336"
                    anchors.verticalCenter: parent.verticalCenter
                }
                Text {
                    id: detectionStatusText
                    text: videoOutput.detectionEnabled ? "Detection: ACTIVE " : "Detection: DISABLED"
                    color: videoOutput.detectionEnabled ? "#4caf50" : "#f44336"
                    font.pixelSize: Qt.platform.os === "android" ? 12 : 11
                    font.bold: true
                    font.family: "sans-serif"
                }
            }

            Row {
                spacing: 10
                Text {
                    id: modelStatusText
                    text: "Model: LOADING..."
                    color: "#ff9800"
                    font.pixelSize: Qt.platform.os === "android" ? 11 : 10
                    font.family: "sans-serif"
                }
            }

            Row {
                spacing: 10
                Text {
                    id: connectionStatusText
                    text: "Connecting to camera..."
                    color: "#ff9800"
                    font.pixelSize: Qt.platform.os === "android" ? 11 : 10
                    font.family: "sans-serif"
                }
            }

            Row {
                spacing: 10
                visible: videoOutput.isConnected && videoOutput.fps > 0
                Text {
                    text: "FPS: " + Math.round(videoOutput.fps)
                    color: "#aaaaaa"
                    font.pixelSize: Qt.platform.os === "android" ? 10 : 9
                    font.family: "sans-serif"
                }
            }
        }
    }

    // Control buttons at bottom
    Rectangle {
        id: controlBar
        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }
        height: Qt.platform.os === "android" ? 70 : 60
        color: "#cc1a1a1a"

        Row {
            anchors.centerIn: parent
            spacing: Qt.platform.os === "android" ? 20 : 15

            Button {
                id: enableDetectionButton
                width: Qt.platform.os === "android" ? 160 : 140
                height: 40
                enabled: videoOutput.isConnected
                opacity: enabled ? 1.0 : 0.5

                background: Rectangle {
                    color: videoOutput.detectionEnabled ? "#f44336" : "#4caf50"
                    radius: 20

                    Rectangle {
                        anchors.fill: parent
                        radius: 20
                        color: parent.parent.pressed ? "#ffffff20" : "transparent"
                    }
                }

                contentItem: Text {
                    text: videoOutput.detectionEnabled ? "DISABLE DETECTION" : "ENABLE DETECTION"
                    color: "#ffffff"
                    font.pixelSize: Qt.platform.os === "android" ? 14 : 12
                    font.bold: true
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }

                onClicked: {
                    if (videoOutput.detectionEnabled) {
                        videoOutput.detectionEnabled = false
                        detectionStatusText.text = "Detection: DISABLED"
                        detectionStatusText.color = "#f44336"
                        console.log("Detection manually disabled")
                        notificationPopup.text = "Detection Disabled"
                        notificationPopup.visible = true
                        notificationPopupTimer.start()
                    } else {
                        videoOutput.detectionEnabled = true
                        detectionStatusText.text = "Detection: ACTIVE "
                        detectionStatusText.color = "#4caf50"
                        console.log("Detection manually enabled")
                        notificationPopup.text = "Detection Enabled - Scanning for pests"
                        notificationPopup.visible = true
                        notificationPopupTimer.start()
                    }
                }
            }

            Button {
                id: reconnectButton
                width: Qt.platform.os === "android" ? 140 : 120
                height: 40
                visible: !videoOutput.isConnected

                background: Rectangle {
                    color: "cyan"
                    radius: 20
                    Rectangle {
                        anchors.fill: parent
                        radius: 20
                        color: parent.parent.pressed ? "#ffffff20" : "transparent"
                    }
                }

                contentItem: Text {
                    text: "RECONNECT"
                    color: "#ffffff"
                    font.pixelSize: Qt.platform.os === "android" ? 14 : 12
                    font.bold: true
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }

                onClicked: {
                    console.log("Manual reconnect triggered")
                    connectionStatusText.text = "Reconnecting..."
                    connectionStatusText.color = "#ff9800"
                    videoOutput.stopProcessing()
                    videoOutput.startProcessing()
                }
            }
        }
    }

    // Floating notification popup
    Rectangle {
        id: notificationPopup
        anchors {
            bottom: controlBar.top
            horizontalCenter: parent.horizontalCenter
            bottomMargin: 10
        }
        width: notificationText.width + 40
        height: 40
        radius: 20
        color: "#cc000000"
        visible: false

        Text {
            id: notificationText
            anchors.centerIn: parent
            text: "Detection Enabled"
            color: "#4caf50"
            font.pixelSize: Qt.platform.os === "android" ? 13 : 11
            font.bold: true
        }

        Timer {
            id: notificationPopupTimer
            interval: 2000
            onTriggered: notificationPopup.visible = false
        }
    }

    // Busy indicator when not connected
    BusyIndicator {
        anchors.centerIn: parent
        running: !videoOutput.isConnected
        visible: running
        width: 80
        height: 80

        Text {
            text: "Connecting to camera..."
            color: "#ffffff"
            font.pixelSize: 14
            anchors.top: parent.bottom
            anchors.topMargin: 10
            anchors.horizontalCenter: parent.horizontalCenter
        }
    }

    // Auto-retry connection timer
    Timer {
        id: retryTimer
        interval: 15000
        repeat: true
        running: !videoOutput.isConnected
        onTriggered: {
            if (!videoOutput.isConnected) {
                console.log("Auto-retrying connection...")
                videoOutput.stopProcessing()
                videoOutput.startProcessing()
            }
        }
    }
}