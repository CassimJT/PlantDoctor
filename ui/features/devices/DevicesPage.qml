import QtQuick 2.15
import QtQuick.Controls 2.15
import "./delegate"
import QtQuick.Layouts

Page {
    id: devicesPage
    padding: 20
    // Message when no devices are connected
    ColumnLayout {
        spacing: 15
        anchors.centerIn: parent

        visible: DeviceModel && DeviceModel.count === 0 &&
                 DeviceConfigurator && !DeviceConfigurator.isScanning
        Text {
            text: "No Devices Connected"
            font.pixelSize: 18
            font.bold: true
            color: "#999"
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Text {
            text: DeviceConfigurator && !DeviceConfigurator.isBrokerConnected
                  ? "Connect to MQTT broker to discover devices"
                  : "Click below to scan for devices"
            font.pixelSize: 14
            color: "#aaa"
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Button {
            id: centerButton
            flat: true
            Layout.alignment: Qt.AlignHCenter
            visible: DeviceModel && DeviceModel.count === 0 &&
                     DeviceConfigurator && !DeviceConfigurator.isScanning

            text: {
                if (!DeviceConfigurator) return "Loading..."
                if (!DeviceConfigurator.isBrokerConnected)
                    return "Connect to Broker"
                else if (DeviceConfigurator.isScanning)
                    return "Scanning..."
                else
                    return "Scan for Devices"
            }

            enabled: DeviceConfigurator &&
                     !DeviceConfigurator.isScanning &&
                     DeviceConfigurator.isBrokerConnected

            onClicked: {
                if (!DeviceConfigurator) return
                if (!DeviceConfigurator.isBrokerConnected) {
                    DeviceConfigurator.connectToBroker()
                } else {
                    DeviceConfigurator.scanForDevices(10)
                }
            }

            width: 200
            height: 50
        }
    }


    ListView {
        id: gridView
        anchors {
            fill: parent
            margins: 15
        }

        model: DeviceModel
        delegate: DeviceDelegate{}

        // Only show when there are devices
        visible: DeviceModel && DeviceModel.count > 0
    }

    // Busy indicator when not connected
    BusyIndicator {
        id: scanningIndicator
        anchors.centerIn: parent
        width: 80
        height: 80
        running: DeviceConfigurator &&
                 DeviceConfigurator.isScanning &&
                 DeviceModel && DeviceModel.count === 0
        visible: running

        Text {
            text: "Connecting to PND Device..."
            color: "#333"
            font.pixelSize: 14
            anchors.top: parent.bottom
            anchors.topMargin: 10
            anchors.horizontalCenter: parent.horizontalCenter
        }
    }

    // Auto-scan when broker connects
    Connections {
        target: DeviceConfigurator

        function onBrokerConnectionChanged() {
            if (DeviceConfigurator && DeviceConfigurator.isBrokerConnected) {
                console.log("Broker connected - auto-scanning for devices")
                DeviceConfigurator.scanForDevices(10)
            }
        }

        function onDeviceDiscovered(deviceId) {
            console.log("Device discovered:", deviceId)
            if (DeviceConfigurator) {
                DeviceConfigurator.connectToDevice(deviceId)
            }
        }

        function onDeviceConnected(deviceId) {
            console.log("Device connected:", deviceId)
        }

        function onDeviceDisconnected(deviceId) {
            console.log("Device disconnected:", deviceId)
        }

        function onErrorOccurred(error) {
            console.error("Device error:", error)
        }
    }

    // Auto-connect to broker on startup
    Component.onCompleted: {
        if (DeviceConfigurator) {
            DeviceConfigurator.connectToBroker()
        }
    }
}
