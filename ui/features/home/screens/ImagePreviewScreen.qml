import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import HistoryModel
import "../../../utils/Utils.js" as Utils
Page {
    id: previewView

    property string capturedImagePath: ""
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

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: photoFrame.bottom
            anchors.topMargin: 14

            text: "Looking good? Tap Analyse to identify"
            color: "#66ffffff"
            font.pointSize: 11
            font.letterSpacing: 0.4
        }

        Column {
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: 24
            anchors.bottomMargin: 48

            spacing: 12

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
                        source: "qrc:/assets/home/icons8-analyse-60.png"
                        width: 24
                        height: 24
                    }

                    Text {
                        text: "Analyse Plant"
                        color: "white"
                        font.pointSize: 16
                        font.bold: true
                    }
                }

                MouseArea {
                    id: analyseArea
                    anchors.fill: parent
                    onClicked: {
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

            Rectangle {
                width: parent.width
                height: 52
                radius: 15
                color: "#02a3b5"

                Row {
                    anchors.centerIn: parent
                    spacing: 8

                    Image {
                        source: "qrc:/assets/home/icons8-retake-100.png"
                        width: 20
                        height: 20
                    }

                    Text {
                        text: "Retake Photo"
                        color: "#ccffffff"
                        font.pointSize: 16
                    }
                }

                MouseArea {
                    anchors.fill: parent

                    onClicked: {
                        mainStackView?.pop()
                        Helper.setIsCamera(true)
                    }
                }
            }
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
            let confidence = InfarenceRunner.confidence  // This is 91.58 (percentage)
            let location = "Zomba"  // to be changed when user adds actual location and variety
            let variaty = diseaseName.split(" ")[0] + "_" + 777

            // Convert percentage to decimal (0-1 range)
            let decimalConfidence = confidence / 100

            console.log("Confidence (percentage):", confidence)
            console.log("Confidence (decimal):", decimalConfidence)

            // Save with decimal confidence (0-1 range)
            HistoryModel.addToHistory(diseaseName, classIndex, currentDate, decimalConfidence, location, variaty)
            console.log(InfarenceRunner.diseaseName)
        }

        function onInfarenceFailed() {
            busyIndicator.visible = false
            busyIndicator.running = false
            console.log("Inference failed")
        }
    }
}