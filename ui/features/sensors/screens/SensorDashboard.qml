import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../../../components"

Page {
    objectName: "Dashboard"
    background: Rectangle { color: "#edf2e0" }

    ColumnLayout {
        anchors {
            fill: parent
            margins: 16
        }
        spacing: 10

        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        RowLayout {
            Layout.fillWidth: true
            height: 170
            spacing: 10

            // Left card — DHT Meter
            Rectangle {
                Layout.fillWidth: true
                height: 170
                radius: 10
                color: "#edf2e0"
                border.color: "black"
                border.width: 1

                DHTMeter {
                    anchors { fill: parent; margins: 8 }
                }
            }

            // Right card — Live Monitor
            Rectangle {
                Layout.fillWidth: true
                height: 170
                radius: 10
                color: "#edf2e0"
                border.color: "black"
                border.width: 1
                clip: true

                Image {
                    anchors.centerIn: parent
                    width: parent.width - 16
                    height: parent.height - 16
                    source: "qrc:/assets/appbar/pest.svg"
                    fillMode: Image.PreserveAspectFit
                    opacity: liveMonitorArea.containsMouse ? 0.7 : 1.0

                    Behavior on opacity {
                        NumberAnimation { duration: 150 }
                    }
                }

                Text {
                    anchors {
                        bottom: parent.bottom
                        horizontalCenter: parent.horizontalCenter
                        bottomMargin: 8
                    }
                    text: "Live Monitor"
                    color: "black"
                    font.pixelSize: 11
                    font.bold: true
                    opacity: 0.8
                }

                MouseArea {
                    id: liveMonitorArea
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        mainStackView?.push("LiveMonitorScreen.qml")
                    }
                }
            }
        }

        // Bottom row — full width card with CircularProgressBar
                RowLayout {
                    Layout.fillWidth: true
                    height: 170
                    spacing: 10

                    Rectangle {
                        Layout.fillWidth: true
                        height: 170
                        radius: 10
                        color: "#edf2e0"
                        border.color: "black"
                        border.width: 1

                        CircularProgressBar {
                            anchors.centerIn: parent
                            width: 140
                            height: 140
                        }
                    }
                }

                Item {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }
            }
        }
