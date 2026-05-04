import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Page {
    id: sysSettings
    objectName: "SystemSettings"
    padding: 12

    ColumnLayout {
        width: parent.width
        anchors {
            top: parent.top
        }

        spacing: 12

        // =======================
        // Broker Settings Group
        // =======================
        GroupBox {
            id: brokerGroupBox
            title: "Broker Settings"
            Layout.fillWidth: true

            ColumnLayout {
                spacing: 10
                Layout.fillWidth: true

                Label {
                    text: "Broker Address:"
                }

                ComboBox {
                    id: brokerCombo
                    Layout.preferredWidth: 150
                    editable: true
                    model: ["192.168.8.130"]

                    onAccepted: {
                        DeviceConfigurator.disconnectFromBroker()
                        DeviceConfigurator.setMqttBroker(editText, 1883)
                        DeviceConfigurator.connectToBroker()
                    }

                    onActivated: {
                        DeviceConfigurator.disconnectFromBroker()
                        DeviceConfigurator.setMqttBroker(model[index], 1883)
                        DeviceConfigurator.connectToBroker()
                    }
                }
            }
        }

        // =======================
        // Language Settings Group
        // =======================
        GroupBox {
            title: "Language Settings"
            Layout.fillWidth: true

            ColumnLayout {
                spacing: 10
                Layout.fillWidth: true

                Label {
                    text: "Select Language:"
                }

                ComboBox {
                    id: languageCombo
                    Layout.fillWidth: true
                    model: ["English","Chichewa"]

                    currentIndex: InfarenceRunner.currentLanguage() === "ny" ? 1 : 0

                    onActivated: {
                        InfarenceRunner.setLanguage(index === 0 ? "en" : "ny")
                    }
                }
            }
        }
    }

    // =======================
    // Dialog for broker connection
    // =======================
    Dialog {
        id: brokerDialog
        title: "Broker Status"
        modal: true
        standardButtons: Dialog.Ok
        anchors.centerIn: parent

        Label {
            anchors.centerIn: parent
            text: brokerDialogMessage
            wrapMode: Text.Wrap
            color: "#333"
            font.weight: Font.Medium
        }
    }

    // Property for dialog message
    property string brokerDialogMessage: ""

    // =======================
    // Connections
    // =======================
    Connections {
        target: DeviceConfigurator

        function onBrokerConnectionChanged() {
            brokerDialogMessage = "Connected to broker"
            brokerDialog.open()
        }

        function onBrokerDisconnected() {
            brokerDialogMessage = "Broker Disconnected"
            brokerDialog.open()
        }

        function onErrorOccurred(message) {
            brokerDialogMessage = message
            brokerDialog.open()
        }
    }
}