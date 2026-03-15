import QtQuick 2.15
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: homescreen
    objectName: "PlantDoctor"
    /* Background reactangle is supposed to be used like below not wrapping everything inside it*/
    background: Rectangle {
        anchors.fill: parent
        color: "#e8efe6"
    }

    ColumnLayout {
        anchors.centerIn: parent


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

        Item {
            Layout.preferredWidth: 10
            Layout.preferredHeight: 30
        }
        // Camera Preview Area
        Rectangle {
            id: cameraField
            Layout.preferredWidth: 260
            Layout.preferredHeight: 320
            radius: 20
            color: "#cfd8cc"
            MouseArea{
                anchors.fill: parent
                onClicked: {
                    cameraField.color = "green"
                }
            }

            // Placeholder image (what camera would show)
            Image {
                anchors.fill: parent
                anchors.margins: 10
                source: "qrc:/assets/home/leaf.png"
                fillMode: Image.PreserveAspectFit
            }

            //animation effect

            Rectangle {
                id: scanLine
                width: parent.width - 40
                height: 3
                color: "#00ff88"
                opacity: 0.9

                radius: 2
                anchors.horizontalCenter: parent.horizontalCenter
                y: 20



                SequentialAnimation {
                    running: true
                    loops: Animation.Infinite

                    NumberAnimation {
                        target: scanLine
                        property: "y"
                        from: 20
                        to: cameraField.height - 20
                        duration: 2000
                        easing.type: Easing.InOutQuad
                    }

                    NumberAnimation {
                        target: scanLine
                        property: "y"
                        from: cameraField.height - 20
                        to: 20
                        duration: 2000
                        easing.type: Easing.InOutQuad
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


                // will have to replace this with an icon
                Image {
                    id: cameraIcon
                    source: "qrc:/assets/home/icons8-camera-90.png"
                    width: 50
                    height: 50
                    fillMode: Image.PreserveAspectFit
                    anchors.centerIn: parent
                    opacity: 0.6
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                         mainLoader.item && mainLoader.item.mainStackView.push("CameraScreen.qml")
                    }
                }
            }

        }

        // Upload Button

        Button{
            id:uploadButton
            text: "Upload Photo"
            icon.source: "qrc:/assets/home/upload-photo-68.png"
            icon.width: 30
            icon.height: 30
            Layout.alignment: Qt.AlignHCenter
            font.pointSize: 18
            font.bold: true
        }


    }

}
