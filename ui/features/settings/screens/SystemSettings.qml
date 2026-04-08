import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Page {
    id: sysSettings
    objectName: "SystemSettings"
    padding: 12
    // =======================
    // Broker Settings Group
    // =======================
    GroupBox {
        title: "Broker Settings"
        width: parent.width
        anchors {
            top: parent.top
            topMargin: 10
        }

        ColumnLayout {
            spacing: 10
            width: parent.width

            Label {
                text: "Broker Address:"
                Layout.alignment: Qt.AlignLeft
            }

            ComboBox {
                id: brokerCombo
                width: parent.width
                editable: true
                model: ["192.168.8.130", "192.168.8.149"]

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
    // Dialog for broker connection
    // =======================
    Dialog {
        id: brokerDialog
        title: "Broker Status"
        modal: true
        standardButtons: Dialog.Ok

        Label {
            anchors.centerIn: parent
            text: brokerDialogMessage
            wrapMode: Text.Wrap
            color: "#333"
            font.weight: Font.Medium
        }

        anchors.centerIn: parent
    }

    // Property for dialog message
    property string brokerDialogMessage: ""

    // =======================
    // Connections to device configurator signals
    // =======================
    Connections {
        target: DeviceConfigurator

        function onBrokerConnectionChanged() {
            brokerDialogMessage = "Connected to broker"
            brokerDialog.open()
        }

        function onBrokerDisconnected() {
            brokerDialogMessage = "Broker Disconneted"
            brokerDialog.open()
        }

        function onErrorOccurred(message) {
            brokerDialogMessage = message
            brokerDialog.open()
        }
    }
}