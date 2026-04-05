import QtQuick 2.15
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs

Page {
    id: homescreen
    background: Rectangle {
        anchors.fill: parent
        color: "#edf2e0"
    }
    topPadding: 50

    ColumnLayout {
        anchors.centerIn: parent
        spacing: 50
        width: parent.width

        //firts contaneir
        ColumnLayout{

            Layout.alignment: Qt.AlignHCenter
            Layout.preferredWidth: parent.width

            // App Logo
            Image {
                id: appLogo
                source: "qrc:/assets/home/logo.png"
                fillMode: Image.PreserveAspectFit
                Layout.preferredWidth: 70
                Layout.preferredHeight: 70
                Layout.alignment: Qt.AlignHCenter
            }

            // Welcome Text
            Text {
                id: welcomeText
                text: qsTr("Welcome!")
                font.bold: true
                font.pointSize: 25
                Layout.alignment: Qt.AlignHCenter
            }

            Text {
                id: hint
                text: qsTr("Tap the camera icon to capture a photo or select from Gallery")
                Layout.preferredWidth: parent.width * 0.7
                wrapMode: Text.WordWrap
                font.pixelSize: 14
                color: "#333"
                Layout.alignment: Qt.AlignHCenter
                horizontalAlignment: Text.AlignHCenter
            }

        }

       //second contaneir
        ColumnLayout{

            Layout.alignment: Qt.AlignHCenter

            // Camera Preview Area
            Rectangle {
                id: cameraField
                Layout.preferredWidth: 260
                Layout.preferredHeight: 320
                radius: 20
                color: "#cfd8cc"
                Layout.alignment: Qt.AlignHCenter
                clip: true

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        Helper.requestCameraPeremision()
                        mainStackView?.push("CameraScreen.qml")
                    }
                }

                // Placeholder image
                Image {
                    anchors.fill: parent
                    anchors.margins: 10
                    source: "qrc:/assets/home/leaf.png"
                    fillMode: Image.PreserveAspectFit
                }

                // scan line matching CameraScreen
                Rectangle {
                    id: scanLine
                    width: parent.width - 40
                    height: 2
                    radius: 1
                    anchors.horizontalCenter: parent.horizontalCenter
                    y: 20

                    gradient: Gradient {
                        orientation: Gradient.Horizontal
                        GradientStop { position: 0.0; color: "transparent" }
                        GradientStop { position: 0.3; color: "#885dde7a" }
                        GradientStop { position: 0.5; color: "#ff5dde7a" }
                        GradientStop { position: 0.7; color: "#885dde7a" }
                        GradientStop { position: 1.0; color: "transparent" }
                    }

                    SequentialAnimation {
                        running: true
                        loops: Animation.Infinite
                        NumberAnimation {
                            target: scanLine; property: "y"
                            from: 20; to: cameraField.height - 20
                            duration: 2200; easing.type: Easing.InOutSine
                        }
                        NumberAnimation {
                            target: scanLine; property: "y"
                            from: cameraField.height - 20; to: 20
                            duration: 2200; easing.type: Easing.InOutSine
                        }
                    }
                }

                // Scanner Frame (corners)
                Item {
                    anchors.fill: parent

                    Repeater {
                        model: 4
                        Rectangle {
                            width: 40
                            height: 3
                            color: "white"
                            property int pos: index
                            anchors {
                                top: pos < 2 ? parent.top : undefined
                                bottom: pos >= 2 ? parent.bottom : undefined
                                left: pos % 2 === 0 ? parent.left : undefined
                                right: pos % 2 === 1 ? parent.right : undefined
                                margins: 15
                            }
                        }
                    }

                    Repeater {
                        model: 4
                        Rectangle {
                            width: 3
                            height: 40
                            color: "white"
                            property int pos: index
                            anchors {
                                top: pos < 2 ? parent.top : undefined
                                bottom: pos >= 2 ? parent.bottom : undefined
                                left: pos % 2 === 0 ? parent.left : undefined
                                right: pos % 2 === 1 ? parent.right : undefined
                                margins: 15
                            }
                        }
                    }
                }

                // Camera Icon Button
                Rectangle {
                    width: 60
                    height: 60
                    radius: 30
                    color: "#ffffff"
                    opacity: 0.4
                    border.color: "white"
                    border.width: 1
                    anchors.centerIn: parent

                    Image {
                        id: cameraIcon
                        source: "qrc:/assets/home/icons8-camera-90.png"
                        width: 50
                        height: 50
                        fillMode: Image.PreserveAspectFit
                        anchors.centerIn: parent
                        opacity: 0.6
                    }
                }
            }
        }

            //Gallery button
        ColumnLayout {
            Layout.alignment: Qt.AlignHCenter

            Rectangle {
                width: 220
                height: 60
                radius: 15
                Layout.alignment: Qt.AlignHCenter

                gradient: Gradient {
                    orientation: Gradient.Horizontal
                    GradientStop { position: 0.0; color: galleryArea.containsPress ? "#2a9e48" : "#34c45a" }
                    GradientStop { position: 1.0; color: galleryArea.containsPress ? "#3dbf60" : "#5dde7a" }
                }

                Rectangle {
                    anchors { top: parent.top; left: parent.left; right: parent.right }
                    height: parent.height / 2
                    radius: parent.radius
                    color: "#1affffff"
                }

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
                        source: "qrc:/assets/home/gallery-96.png"
                        width: 24; height: 24
                        anchors.verticalCenter: parent.verticalCenter
                    }
                    Text {
                        text: "Select from Gallery"
                        color: "white"
                        font.pointSize: 14
                        font.bold: true
                        font.letterSpacing: 0.6
                        anchors.verticalCenter: parent.verticalCenter
                    }
                }

                scale: galleryArea.containsPress ? 0.97 : 1.0
                Behavior on scale { NumberAnimation { duration: 100; easing.type: Easing.OutQuad } }

                MouseArea {
                    id: galleryArea
                    anchors.fill: parent
                    onClicked: {
                        fileDialog.open()
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

            //----connection section ---------------
            Connections {
                target: Helper
                function onImageReady() {
                    var preview = Helper.imagePreview()
                    var path = Helper.localFilePath()
                    
                }
            }

    }
}
