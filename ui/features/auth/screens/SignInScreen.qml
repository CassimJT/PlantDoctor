import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Page {
    id: signInScreen
    width: 360
    height: 800
    background: Rectangle { color: "#e8efe6" }

    property var onLoginSuccess: null

    Flickable {
        anchors.fill: parent
        contentHeight: mainColumn.implicitHeight + 60
        clip: true

        ColumnLayout {
            id: mainColumn
            width: parent.width
            spacing: 0

            Item { Layout.preferredHeight: 100 }

            // App Logo
            Image {
                id: appLogo
                source: "qrc:/assets/home/logo.png"
                fillMode: Image.PreserveAspectFit

                Layout.preferredWidth: 40
                Layout.preferredHeight: 40
                Layout.alignment: Qt.AlignHCenter
            }

            // ── Heading ─────────────────────────────────────────────────────
            Text {
                Layout.alignment: Qt.AlignHCenter
                text: "Sign in to PlantDoctor"
                font.family: "Georgia"
                font.pixelSize: 26
                font.bold: true
                color: "#1A2E1F"
                font.letterSpacing: 0.3
            }

            // Sign in message
            Text {
                Layout.alignment: Qt.AlignHCenter
                text: "Sign in for uninterrupted use"
                font.pixelSize: 14
                color: "#666"
                font.italic: true
                visible: !AppSettings.isLoggedIn()
                Layout.topMargin: 8
            }

            Item { Layout.preferredHeight: 70 }

            // ── Card ─────────────────────────────────────────────────────────
            Rectangle {
                Layout.leftMargin: 20
                Layout.rightMargin: 20
                Layout.fillWidth: true
                implicitHeight: cardColumn.implicitHeight + 36
                radius: 20
                color: "#FFFFFF"
                border.color: "#000000"
                border.width: 1

                ColumnLayout {
                    id: cardColumn
                    anchors {
                        top: parent.top
                        left: parent.left
                        right: parent.right
                        topMargin: 28
                        leftMargin: 20
                        rightMargin: 20
                    }
                    spacing: 16

                    // ── Phone Number field ───────────────────────────────────
                    Rectangle {
                        Layout.fillWidth: true
                        height: 56
                        radius: 12
                        color: "#FAFAFA"
                        border.color: phoneField.activeFocus ? "#8FAF8F" : "#000000"
                        border.width: phoneField.activeFocus ? 1.5 : 1
                        enabled: !ApiClient.isloading

                        Behavior on border.color { ColorAnimation { duration: 150 } }

                        Text {
                            id: phoneLabel
                            text: "Phone number"
                            color: "#000000"
                            font.pixelSize: 14
                            anchors.verticalCenter: parent.verticalCenter
                            x: 16
                            visible: phoneField.text.length === 0 && !phoneField.activeFocus
                        }

                        TextField {
                            id: phoneField
                            anchors {
                                fill: parent
                                leftMargin: 16
                                rightMargin: 14
                            }
                            placeholderText: ""
                            color: "#000000"
                            font.pixelSize: 14
                            background: Item {}
                            inputMethodHints: Qt.ImhDialableCharactersOnly
                            verticalAlignment: TextInput.AlignVCenter
                            enabled: !ApiClient.isloading
                        }
                    }

                    // ── District / Location field ─────────────────────────────
                    Rectangle {
                        Layout.fillWidth: true
                        height: 56
                        radius: 12
                        color: "#FAFAFA"
                        border.color: districtField.activeFocus ? "#8FAF8F" : "#000000"
                        border.width: districtField.activeFocus ? 1.5 : 1
                        enabled: !ApiClient.isloading

                        Behavior on border.color { ColorAnimation { duration: 150 } }

                        Text {
                            id: districtLabel
                            text: "District"
                            color: "#000000"
                            font.pixelSize: 14
                            anchors.verticalCenter: parent.verticalCenter
                            x: 16
                            visible: districtField.text.length === 0 && !districtField.activeFocus
                        }

                        TextField {
                            id: districtField
                            anchors {
                                fill: parent
                                leftMargin: 16
                                rightMargin: 14
                            }
                            placeholderText: ""
                            color: "#000000"
                            font.pixelSize: 14
                            background: Item {}
                            verticalAlignment: TextInput.AlignVCenter
                            enabled: !ApiClient.isloading
                        }
                    }

                    // Error message
                    Text {
                        id: errorMessage
                        text: ""
                        color: "red"
                        font.pixelSize: 12
                        visible: false
                        Layout.alignment: Qt.AlignHCenter
                    }

                    // Loading indicator and message
                    Column {
                        Layout.fillWidth: true
                        spacing: 8
                        visible: ApiClient.isloading
                        Layout.alignment: Qt.AlignHCenter

                        BusyIndicator {
                            id: busyIndicator
                            visible: ApiClient.isloading
                            running: ApiClient.isloading
                            Layout.alignment: Qt.AlignHCenter
                            width: 40
                            height: 40
                        }

                        Text {
                            text: "Signing you in..."
                            font.pixelSize: 12
                            color: "#666"
                            font.italic: true
                            Layout.alignment: Qt.AlignHCenter
                            visible: ApiClient.isloading
                        }
                    }

                    Item { Layout.preferredHeight: 4 }

                    // ── Continue button ───────────────────────────────────────
                    Rectangle {
                        Layout.fillWidth: true
                        height: 52
                        radius: 14
                        color: continueMA.pressed ? "#7A9E7E" : "#8FAF8F"
                        opacity: ApiClient.isloading ? 0.6 : 1.0

                        Behavior on color { ColorAnimation { duration: 120 } }

                        Text {
                            anchors.centerIn: parent
                            text: ApiClient.isloading ? "Please wait..." : "Continue"
                            font.family: "Georgia"
                            font.pixelSize: 16
                            color: "#FFFFFF"
                            font.letterSpacing: 0.4
                        }

                        MouseArea {
                            id: continueMA
                            anchors.fill: parent
                            enabled: !ApiClient.isloading
                            onClicked: {
                                var phone = phoneField.text.trim()
                                var district = districtField.text.trim()

                                if (phone.length === 0) {
                                    errorMessage.text = "Please enter your phone number"
                                    errorMessage.visible = true
                                    return
                                }

                                if (district.length === 0) {
                                    errorMessage.text = "Please enter your district"
                                    errorMessage.visible = true
                                    return
                                }

                                errorMessage.visible = false

                                // Register user
                                ApiClient.registerUser(phone, district)
                            }
                        }
                    }

                    Item { Layout.preferredHeight: 8 }
                }
            }

            Item { Layout.preferredHeight: 40 }
        }
    }

    Connections {
        target: ApiClient
        // In SignInScreen.qml Connections for ApiClient
        function onLoginFinished(success, response) {
            if (success) {
                console.log("Registration successful!")
                AppSettings.setInferenceCounter(0)

                // Pop back to previous screen
                if (mainStackView) {
                    mainStackView.pop()
                }
            } else {
                var message = response.message ? response.message : "Registration failed"
                errorMessage.text = message
                errorMessage.visible = true
                console.log("Registration failed:", message)
            }
        }

    }

}