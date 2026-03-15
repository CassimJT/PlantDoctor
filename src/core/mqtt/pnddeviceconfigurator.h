#ifndef PNDDEVICECONFIGURATOR_H
#define PNDDEVICECONFIGURATOR_H

#include <QObject>
#include <QTimer>
#include <QJsonObject>
#include <QSet>
#include <QDateTime>
#include <QHash>
#include "mqttclient.h"
#include "pnddevicemodel.h"
#include "pndtopics.h"
#include "pnddevicestate.h"

class PNDDeviceConfigurator : public QObject
{
    Q_OBJECT
    Q_PROPERTY(PNDDeviceModel* deviceModel READ deviceModel CONSTANT)
    Q_PROPERTY(bool isScanning READ isScanning NOTIFY scanningChanged)
    Q_PROPERTY(int discoveredDeviceCount READ discoveredDeviceCount NOTIFY discoveredDevicesChanged)

public:
    explicit PNDDeviceConfigurator(QObject *parent = nullptr);
    ~PNDDeviceConfigurator();

    // Properties
    PNDDeviceModel* deviceModel() const;
    bool isScanning() const;
    int discoveredDeviceCount() const;

    // MQTT Configuration
    void setMqttBroker(const QString &host, int port);
    void setMqttCredentials(const QString &username, const QString &password);

public slots:
    // Device management
    void scanForDevices(int timeoutSeconds = 10);
    void stopScan();
    void connectToDevice(const QString &deviceId);
    void disconnectDevice(const QString &deviceId);
    void setDevicePower(const QString &deviceId, bool powerOn);
    void configureDevice(const QString &deviceId, const QJsonObject &config);
    void requestDeviceStatus(const QString &deviceId);
    void requestDeviceSensors(const QString &deviceId);

    // MQTT connection
    void connectToBroker();
    void disconnectFromBroker();

signals:
    void deviceDiscovered(const QString &deviceId);
    void deviceConnected(const QString &deviceId);
    void deviceDisconnected(const QString &deviceId);
    void deviceStatusUpdated(const QString &deviceId, const QJsonObject &status);
    void deviceSensorsUpdated(const QString &deviceId, float temperature, float humidity);
    void errorOccurred(const QString &message);
    void scanningChanged();
    void discoveredDevicesChanged();
    void brokerConnected();
    void brokerDisconnected();

private slots:
    void handleMqttMessage(const QString &topic, const QByteArray &payload);
    void handleMqttConnected();
    void handleMqttDisconnected();
    void handleMqttError(const QString &error);
    void onScanTimeout();
    void checkDeviceAvailability();

private:
    void subscribeToTopics();
    void processDiscoveryMessage(const QString &topic, const QByteArray &payload);
    void processStatusMessage(const QString &deviceId, const QByteArray &payload);
    void processSensorsMessage(const QString &deviceId, const QByteArray &payload);
    void processErrorMessage(const QString &deviceId, const QByteArray &payload);
    void processAvailabilityMessage(const QString &deviceId, const QByteArray &payload);

    void updateDeviceState(const QString &deviceId, PNDDeviceState::State state);
    void publishCommand(const QString &deviceId, const QString &command, const QJsonObject &payload = QJsonObject());

    MQTTClient *m_mqttClient;
    PNDDeviceModel *m_deviceModel;

    QTimer *m_scanTimer;
    QTimer *m_availabilityTimer;
    bool m_isScanning;
    int m_availabilityTimeout;  // seconds

    QSet<QString> m_discoveredDevices;
    QHash<QString, QDateTime> m_lastSeen;  // Track when devices were last seen

    // Command tracking
    struct PendingCommand {
        QString command;
        QDateTime timestamp;
        QJsonObject data;
    };
    QHash<QString, PendingCommand> m_pendingCommands;
};

#endif // PNDDEVICECONFIGURATOR_H
