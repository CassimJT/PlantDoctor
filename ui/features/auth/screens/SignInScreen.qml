import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Page {
    id: signInScreen
    width: 360
    height: 800

    background: Rectangle {
        color: "#edf2e0"
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 0

        Item {
            Layout.preferredHeight: 100
        }

        // App Logo
        Image {
            id: appLogo
            source: "qrc:/assets/home/logo.png"
            fillMode: Image.PreserveAspectFit

            Layout.preferredWidth: 40
            Layout.preferredHeight: 40
            Layout.alignment: Qt.AlignHCenter
        }

        // Heading
        Text {
            Layout.alignment: Qt.AlignHCenter
            text: "Sign in to PlantDoctor"

            font.family: "Georgia"
            font.pixelSize: 26
            font.bold: true
            font.letterSpacing: 0.3

            color: "#1A2E1F"
        }

        Item {
            Layout.preferredHeight: 70
        }

        // Card
        Rectangle {
            Layout.fillWidth: true
            radius: 20

            color: "#FFFFFF"
            border.color: "#000000"
            border.width: 1

            implicitHeight: cardColumn.implicitHeight + 40

            ColumnLayout {
                id: cardColumn

                anchors {
                    fill: parent
                    margins: 20
                }

                spacing: 18

                // Phone Number Field
                Rectangle {
                    Layout.fillWidth: true
                    height: 56
                    radius: 12

                    color: "#FAFAFA"
                    border.color: phoneField.activeFocus ? "#8FAF8F" : "#000000"
                    border.width: phoneField.activeFocus ? 1.5 : 1

                    Text {
                        text: "Phone Number"
                        color: "#000000"
                        font.pixelSize: 14

                        anchors.verticalCenter: parent.verticalCenter
                        x: 16

                        visible: phoneField.text.length === 0
                                 && !phoneField.activeFocus
                    }

                    TextField {
                        id: phoneField

                        anchors.fill: parent
                        anchors.leftMargin: 16
                        anchors.rightMargin: 14

                        background: Item {}

                        color: "#000000"
                        font.pixelSize: 14

                        inputMethodHints: Qt.ImhDialableCharactersOnly
                        verticalAlignment: TextInput.AlignVCenter
                    }
                }

                // Location Field
                Rectangle {
                    Layout.fillWidth: true
                    height: 56
                    radius: 12

                    color: "#FAFAFA"
                    border.color: locationField.activeFocus ? "#8FAF8F" : "#000000"
                    border.width: locationField.activeFocus ? 1.5 : 1

                    Text {
                        text: "Location"
                        color: "#000000"
                        font.pixelSize: 14

                        anchors.verticalCenter: parent.verticalCenter
                        x: 16

                        visible: locationField.text.length === 0
                                 && !locationField.activeFocus
                    }

                    TextField {
                        id: locationField

                        anchors.fill: parent
                        anchors.leftMargin: 16
                        anchors.rightMargin: 14

                        background: Item {}

                        color: "#000000"
                        font.pixelSize: 14

                        verticalAlignment: TextInput.AlignVCenter
                    }
                }

                Item {
                    Layout.preferredHeight: 8
                }

                // Continue Button
                Rectangle {
                    Layout.fillWidth: true
                    height: 52
                    radius: 14

                    gradient: Gradient {
                        orientation: Gradient.Horizontal

                        GradientStop {
                            position: 0.0
                            color: continueMA.pressed ? "#2a9e48" : "#34c45a"
                        }

                        GradientStop {
                            position: 1.0
                            color: continueMA.pressed ? "#3dbf60" : "#5dde7a"
                        }
                    }

                    Text {
                        anchors.centerIn: parent
                        text: "Continue"

                        font.family: "Georgia"
                        font.pixelSize: 16
                        font.letterSpacing: 0.4

                        color: "#FFFFFF"
                    }

                    MouseArea {
                        id: continueMA
                        anchors.fill: parent

                        onClicked: {
                            mainStackView?.push("home/screens/HomeScreen.qml")
                        }
                    }
                }
            }
        }
    }
}
