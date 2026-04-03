import QtQuick 2.15
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: homescreen

    background: Rectangle {
        anchors.fill: parent
        color: "#edf2e0"
    }

    ColumnLayout {
        anchors.centerIn: parent
        spacing: 50
        width: parent.width


        ColumnLayout{
            //firts contaneir
            Layout.alignment: Qt.AlignHCenter
            Layout.preferredWidth: parent.width

            // App Logo
            Image {
                id: appLogo
                source: "qrc:/assets/home/logo.png"
                fillMode: Image.PreserveAspectFit
                Layout.preferredWidth: 80
                Layout.preferredHeight: 80
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
                font.pixelSize: 16
                Layout.alignment: Qt.AlignHCenter
                horizontalAlignment: Text.AlignHCenter
            }

        }

        /*
        Item {
            Layout.preferredWidth: 10
            Layout.preferredHeight: 50
        }
        */
        ColumnLayout{
            //second contaneir
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

                // Improved scan line matching CameraScreen
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

       ColumnLayout{
           //last cont
           Layout.alignment: Qt.AlignHCenter

           Button {
               id: uploadButton
               text: "Gallery"

               icon.source: "qrc:/assets/home/gallery-96.png"
               icon.width: 30
               icon.height: 30

               Layout.alignment: Qt.AlignHCenter
               font.pointSize: 18
               font.bold: true
               background: Rectangle{
                   anchors.fill: parent
                   color: "#21b054"
                   radius: 10
               }
           }

       }


        // Select from Gallery button

    }
}
