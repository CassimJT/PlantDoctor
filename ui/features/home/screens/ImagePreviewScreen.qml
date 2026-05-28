import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import HistoryModel
import QtQuick.Dialogs
import "../../../utils/Utils.js" as Utils

Page {
    id: previewView

    property string capturedImagePath: ""
    property bool isProcessingInference: false
    topPadding: 90

    contentItem: Item {
        anchors.fill: parent

        Rectangle {
            anchors.fill: parent
            color: "#edf2e0"
        }

        RowLayout {
            id: previewTopBar
            width: parent.width

            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
                margins: 20
                topMargin: 70
                horizontalCenter: parent.horizontalCenter
            }
            Rectangle {
                height: 32
                width: previewLabel.implicitWidth + 24
                radius: 16
                color: "#f5f5f5"
                border.color: "#44ffffff"
                border.width: 1

                Text {
                    id: previewLabel
                    anchors.centerIn: parent
                    text: "Image Preview"
                    font.pointSize: 11
                    font.letterSpacing: 1.2
                }
            }

            Item {
                Layout.fillWidth: true
            }

            Item {
                width: 42
                height: 42
            }
        }

        Item {
            id: photoFrame

            width: parent.width - 48
            height: parent.height * 0.58

            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: previewTopBar.bottom
            anchors.topMargin: 20

            // Hidden image used by Canvas
            Image {
                id: loaderImage
                visible: false
                source: roundedCanvas.imageSource

                onStatusChanged: {
                    if (status === Image.Ready) {
                        roundedCanvas.loading = false
                        roundedCanvas.requestPaint()
                    }
                }
            }

            Canvas {
                id: roundedCanvas

                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: capturedLabel.top
                anchors.margins: 10

                property url imageSource: ""
                property bool loading: false

                onPaint: {
                    var ctx = getContext("2d")
                    ctx.clearRect(0, 0, width, height)

                    var radius = 12

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

                    if (loaderImage.status === Image.Ready) {
                        ctx.drawImage(loaderImage, 0, 0, width, height)
                    }
                }
            }

            BusyIndicator {
                id: busyIndicator
                anchors.centerIn: roundedCanvas
                visible: true
                running: true
                width: 40
                height: 40
                z: 10
            }

            Item {
                id: capturedLabel

                anchors.bottom: parent.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.margins: 10
                anchors.bottomMargin: 10

                height: 28
            }
        }

        Column {
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: 24
            anchors.bottomMargin: 48

            spacing: 16

            // Retake + Gallery round buttons row
            Row {
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: 48

                // Retake
                Column {
                    spacing: 8
                    anchors.verticalCenter: parent.verticalCenter

                    Rectangle {
                        width: 64; height: 64; radius: 32
                        anchors.horizontalCenter: parent.horizontalCenter
                        color: retakeArea.containsPress ? "#cfd8cc" : "#cfd8cc"
                        border.color: "#cccccc"; border.width: 1
                        Behavior on color { ColorAnimation { duration: 120 } }

                        Image {
                            source: "qrc:/assets/home/retake.png"
                            width: 30; height: 30
                            anchors.centerIn: parent
                        }

                        MouseArea {
                            id: retakeArea
                            anchors.fill: parent
                            onClicked: {
                                mainStackView?.pop()
                                Helper.setIsCamera(true)
                            }
                        }
                    }

                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: "Retake"
                        color: "#445544"
                        font.pointSize: 12
                        font.letterSpacing: 0.3
                    }
                }

                // Gallery
                Column {
                    spacing: 8
                    anchors.verticalCenter: parent.verticalCenter

                    Rectangle {
                        width: 64; height: 64; radius: 32
                        anchors.horizontalCenter: parent.horizontalCenter
                        color: galleryPressArea.containsPress ? "#ddd8cc" : "#cfd8cc"
                        border.color: "#cccccc"; border.width: 1
                        Behavior on color { ColorAnimation { duration: 120 } }

                        Image {
                            source: "qrc:/assets/home/gallery.png"
                            width: 30; height: 30
                            anchors.centerIn: parent
                        }

                        MouseArea {
                            id: galleryPressArea
                            anchors.fill: parent
                            onClicked: {
                                fileDialog.open()
                            }
                        }
                    }

                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: "Gallery"
                        color: "#445544"
                        font.pointSize: 12
                        font.letterSpacing: 0.3
                    }
                }
            }

            //analysis
            Rectangle {
                width: parent.width
                height: 60
                radius: 15

                gradient: Gradient {
                    orientation: Gradient.Horizontal

                    GradientStop {
                        position: 0.0
                        color: analyseArea.containsPress ? "#2a9e48" : "#34c45a"
                    }

                    GradientStop {
                        position: 1.0
                        color: analyseArea.containsPress ? "#3dbf60" : "#5dde7a"
                    }
                }

                Row {
                    anchors.centerIn: parent
                    spacing: 10

                    Image {
                        source: "qrc:/assets/home/diagnosis.png"
                        width: 30
                        height: 30
                    }

                    Text {
                        text: "Analyse Plant"
                        color: "white"
                        font.pointSize: 18
                        font.bold: true
                    }
                }

                MouseArea {
                    id: analyseArea
                    anchors.fill: parent
                    onClicked: {
                        // Check if user is logged in
                        if (!AppSettings.isLoggedIn()) {
                            // Check inference counter
                            var currentCounter = AppSettings.inferenceCounter()
                            if (currentCounter >= 5) {
                                // Push login screen
                                mainStackView?.push("qrc:/qt/qml/PlantDoctor/ui/features/auth/screens/SignInScreen.qml", {
                                    onLoginSuccess: function() {
                                        console.log("Login successful, resetting counter")
                                        AppSettings.setInferenceCounter(0)
                                        // Retry the analysis after login
                                        Utils.uploadForInfarance()
                                        busyIndicator.visible = true
                                        busyIndicator.running = true
                                        mainStackView?.push(
                                            "qrc:/qt/qml/PlantDoctor/ui/features/inference/screens/InfarenceResultScreen.qml",
                                            { "imagePath": roundedCanvas.imageSource }
                                        )
                                    }
                                })
                                return
                            }
                        }

                        // Proceed with inference
                        Utils.uploadForInfarance()
                        busyIndicator.visible = true
                        busyIndicator.running = true
                        mainStackView?.push(
                            "qrc:/qt/qml/PlantDoctor/ui/features/inference/screens/InfarenceResultScreen.qml",
                            { "imagePath": roundedCanvas.imageSource }
                        )
                    }
                }
            }
        }
    }

    //--------dialog section-----
    FileDialog {
        id: fileDialog
        title: "Select an Image"
        onAccepted: {
            console.log("Selected file:", fileDialog.selectedFile);
            var path = fileDialog.selectedFile
            Helper.loadImageFromContentUri(path)
        }

        onRejected: {
            console.log("File selection canceled.");
        }
    }

    Connections {
        target: Helper

        function onImageReady() {
            busyIndicator.visible = false
            busyIndicator.running = false
            var preview = Helper.imagePreview()
            var path = Helper.localFilePath()
            var src = path !== "" ? path : preview

            roundedCanvas.imageSource = src
            roundedCanvas.loading = true
        }
    }

    Connections {
        target: InfarenceRunner

        function onInfarenceFinished() {
            busyIndicator.visible = false
            busyIndicator.running = false

            let diseaseName = InfarenceRunner.diseaseName
            let classIndex = InfarenceRunner.classIndex
            let currentDate = Utils.getCurrentDate()
            let confidence = InfarenceRunner.confidence
            let location = AppSettings.getUserDistrict() !== "" ? AppSettings.getUserDistrict() : "N/A"
            let variaty = diseaseName.split(" ")[0] + "_" + 777

            // Convert percentage to decimal (0-1 range)
            let decimalConfidence = confidence / 100

            console.log("Confidence (percentage):", confidence)
            console.log("Confidence (decimal):", decimalConfidence)
            console.log("Location:", location)

            // Save with decimal confidence (0-1 range)
            HistoryModel.addToHistory(diseaseName, classIndex, currentDate, decimalConfidence, location, variaty)
            console.log(InfarenceRunner.diseaseName)
        }

        function onInfarenceFailed() {
            busyIndicator.visible = false
            busyIndicator.running = false
            console.log("Inference failed")
        }

        function onInferenceLimitReached() {
            console.log("Inference limit reached, showing login screen")
            mainStackView?.push("qrc:/qt/qml/PlantDoctor/ui/features/auth/screens/SignInScreen.qml")
        }
    }
}