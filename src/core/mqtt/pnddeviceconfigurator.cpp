#include "pnddeviceconfigurator.h"
#include <QJsonDocument>
#include <QJsonValue>
#include <QDateTime>
#include <QDebug>

PNDDeviceConfigurator::PNDDeviceConfigurator(QObject *parent)
    : QObject(parent)
    , m_mqttClient(new MQTTClient(this))
    , m_deviceModel(new PNDDeviceModel(this))
    , m_scanTimer(new QTimer(this))
    , m_availabilityTimer(new QTimer(this))
    , m_isScanning(false)
    , m_availabilityTimeout(120)  // 2 minutes default
{
    // Connect MQTT signals
    connect(m_mqttClient, &MQTTClient::connected,
            this, &PNDDeviceConfigurator::handleMqttConnected);
    connect(m_mqttClient, &MQTTClient::disconnected,
            this, &PNDDeviceConfigurator::handleMqttDisconnected);
    connect(m_mqttClient, &MQTTClient::errorOccurred,
            this, &PNDDeviceConfigurator::handleMqttError);
    connect(m_mqttClient, &MQTTClient::messageReceived,
            this, &PNDDeviceConfigurator::handleMqttMessage);

    // Setup scan timer
    m_scanTimer->setSingleShot(true);
    connect(m_scanTimer, &QTimer::timeout, this, &PNDDeviceConfigurator::onScanTimeout);

    // Setup availability checking
    m_availabilityTimer->setInterval(30000);  // Check every 30 seconds
    connect(m_availabilityTimer, &QTimer::timeout,
            this, &PNDDeviceConfigurator::checkDeviceAvailability);

    // Set default broker
    m_mqttClient->setHost("192.168.8.130");
    m_mqttClient->setPort(1883);
}

PNDDeviceConfigurator::~PNDDeviceConfigurator()
{
    stopScan();
    m_availabilityTimer->stop();
    disconnectFromBroker();
}

// Properties
PNDDeviceModel* PNDDeviceConfigurator::deviceModel() const
{
    return m_deviceModel;
}

bool PNDDeviceConfigurator::isScanning() const
{
    return m_isScanning;
}

int PNDDeviceConfigurator::discoveredDeviceCount() const
{
    return m_discoveredDevices.size();
}

// MQTT Configuration
void PNDDeviceConfigurator::setMqttBroker(const QString &host, int port)
{
    m_mqttClient->setHost(host);
    m_mqttClient->setPort(port);
}

void PNDDeviceConfigurator::setMqttCredentials(const QString &username, const QString &password)
{
    m_mqttClient->setUsername(username);
    m_mqttClient->setPassword(password);
}

// Public slots
void PNDDeviceConfigurator::scanForDevices(int timeoutSeconds)
{
    if (m_isScanning) {
        return;
    }

    m_isScanning = true;
    m_discoveredDevices.clear();
    emit scanningChanged();
    emit discoveredDevicesChanged();

    // Subscribe to discovery topic
    if (m_mqttClient->isConnected()) {
        m_mqttClient->subscribe(PNDTopics::discoveryTopic());

        // Publish discovery request
        QJsonObject request;
        request["command"] = "discover";
        request["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);

        m_mqttClient->publish(
            PNDTopics::discoveryTopic(),
            QJsonDocument(request).toJson()
            );
    }

    // Start timeout timer
    m_scanTimer->start(timeoutSeconds * 1000);
}

void PNDDeviceConfigurator::stopScan()
{
    if (m_isScanning) {
        m_scanTimer->stop();
        m_isScanning = false;
        emit scanningChanged();
    }
}

void PNDDeviceConfigurator::connectToDevice(const QString &deviceId)
{
    if (!m_deviceModel->contains(deviceId)) {
        m_deviceModel->addDevice(deviceId);
    }

    PNDDevice *device = m_deviceModel->getDevice(deviceId);
    if (device) {
        updateDeviceState(deviceId, PNDDeviceState::CONNECTING);

        // Subscribe to device topics
        m_mqttClient->subscribe(PNDTopics::deviceStatusTopic(deviceId));
        m_mqttClient->subscribe(PNDTopics::deviceSensorsTopic(deviceId));
        m_mqttClient->subscribe(PNDTopics::deviceAvailabilityTopic(deviceId));
        m_mqttClient->subscribe(PNDTopics::deviceConfigTopic(deviceId));

        // Request status
        requestDeviceStatus(deviceId);
    }
}

void PNDDeviceConfigurator::disconnectDevice(const QString &deviceId)
{
    PNDDevice *device = m_deviceModel->getDevice(deviceId);
    if (device) {
        updateDeviceState(deviceId, PNDDeviceState::DISCONNECTED);

        // Unsubscribe from device topics
        m_mqttClient->unsubscribe(PNDTopics::deviceStatusTopic(deviceId));
        m_mqttClient->unsubscribe(PNDTopics::deviceSensorsTopic(deviceId));
        m_mqttClient->unsubscribe(PNDTopics::deviceAvailabilityTopic(deviceId));
        m_mqttClient->unsubscribe(PNDTopics::deviceConfigTopic(deviceId));

        // Remove from last seen tracking
        m_lastSeen.remove(deviceId);
    }
}

void PNDDeviceConfigurator::setDevicePower(const QString &deviceId, bool powerOn)
{
    QJsonObject command;
    command["command"] = "power";
    command["value"] = powerOn;

    publishCommand(deviceId, "power", command);
}

void PNDDeviceConfigurator::configureDevice(const QString &deviceId, const QJsonObject &config)
{
    QJsonObject command;
    command["command"] = "configure";
    command["config"] = config;

    publishCommand(deviceId, "configure", command);
}

void PNDDeviceConfigurator::requestDeviceStatus(const QString &deviceId)
{
    QJsonObject command;
    command["command"] = "get_status";

    publishCommand(deviceId, "get_status", command);
}

void PNDDeviceConfigurator::requestDeviceSensors(const QString &deviceId)
{
    QJsonObject command;
    command["command"] = "get_sensors";

    publishCommand(deviceId, "get_sensors", command);
}

void PNDDeviceConfigurator::connectToBroker()
{
    m_mqttClient->connectToBroker();
}

void PNDDeviceConfigurator::disconnectFromBroker()
{
    m_availabilityTimer->stop();
    m_mqttClient->disconnectFromBroker();
}

// Private slots
void PNDDeviceConfigurator::handleMqttMessage(const QString &topic, const QByteArray &payload)
{
    qDebug() << "MQTT Message - Topic:" << topic << "Payload:" << payload;

    QString deviceId = PNDTopics::extractDeviceId(topic);

    if (!deviceId.isEmpty()) {
        // Update last seen timestamp for any message from this device
        m_lastSeen[deviceId] = QDateTime::currentDateTime();
    }

    if (topic == PNDTopics::discoveryTopic()) {
        processDiscoveryMessage(topic, payload);
    } else if (!deviceId.isEmpty()) {
        if (topic.endsWith(PNDTopics::STATUS)) {
            processStatusMessage(deviceId, payload);
        } else if (topic.endsWith(PNDTopics::SENSORS)) {
            processSensorsMessage(deviceId, payload);
        } else if (topic.endsWith(PNDTopics::ERROR_TOPIC)) {
            processErrorMessage(deviceId, payload);
        } else if (topic.endsWith(PNDTopics::AVAILABILITY)) {
            processAvailabilityMessage(deviceId, payload);
        }
    }
}

void PNDDeviceConfigurator::handleMqttConnected()
{
    qDebug() << "Connected to MQTT broker";

    // Subscribe to global topics
    m_mqttClient->subscribe(PNDTopics::discoveryTopic());

    // Subscribe to all availability topics to catch LWT messages
    m_mqttClient->subscribe("plantdoctor/device/+/availability");

    // Start availability timer
    m_availabilityTimer->start();

    // Resubscribe to all active devices
    for (int i = 0; i < m_deviceModel->rowCount(); ++i) {
        PNDDevice *device = m_deviceModel->getDevice(i);
        if (device && device->state() != PNDDeviceState::DISCONNECTED) {
            connectToDevice(device->deviceId());
        }
    }

    emit brokerConnected();
}

void PNDDeviceConfigurator::handleMqttDisconnected()
{
    qDebug() << "Disconnected from MQTT broker";

    m_availabilityTimer->stop();

    // Update all device states to disconnected
    for (int i = 0; i < m_deviceModel->rowCount(); ++i) {
        PNDDevice *device = m_deviceModel->getDevice(i);
        if (device && device->state() == PNDDeviceState::CONNECTED) {
            updateDeviceState(device->deviceId(), PNDDeviceState::DISCONNECTED);
        }
    }

    m_lastSeen.clear();

    emit brokerDisconnected();
}

void PNDDeviceConfigurator::handleMqttError(const QString &error)
{
    emit errorOccurred("MQTT Error: " + error);
}

void PNDDeviceConfigurator::onScanTimeout()
{
    stopScan();
}

void PNDDeviceConfigurator::checkDeviceAvailability()
{
    QDateTime now = QDateTime::currentDateTime();
    qDebug() << "Checking device availability. Tracked devices:" << m_lastSeen.size();

    for (auto it = m_lastSeen.begin(); it != m_lastSeen.end(); ) {
        QString deviceId = it.key();
        QDateTime lastSeen = it.value();
        qint64 secondsSince = lastSeen.secsTo(now);

        PNDDevice *device = m_deviceModel->getDevice(deviceId);
        if (!device) {
            // Device no longer in model, remove from tracking
            it = m_lastSeen.erase(it);
            continue;
        }

        // If device was connected but we haven't seen it
        if (device->state() == PNDDeviceState::CONNECTED) {
            if (secondsSince > m_availabilityTimeout) {
                qDebug() << "Device" << deviceId
                         << "not seen for" << secondsSince
                         << "seconds (> " << m_availabilityTimeout << ")";

                // Force disconnect
                updateDeviceState(deviceId, PNDDeviceState::DISCONNECTED);
                emit deviceDisconnected(deviceId);

                // Remove from tracking (will be re-added if it comes back)
                it = m_lastSeen.erase(it);
                continue;
            }
        }

        ++it;
    }
}

// Private methods
void PNDDeviceConfigurator::subscribeToTopics()
{
    m_mqttClient->subscribe(PNDTopics::discoveryTopic());
    m_mqttClient->subscribe("plantdoctor/device/+/availability");  // For LWT
}

void PNDDeviceConfigurator::processDiscoveryMessage(const QString &topic, const QByteArray &payload)
{
    qDebug() << "Processing discovery:" << payload;

    QJsonDocument doc = QJsonDocument::fromJson(payload);
    if (!doc.isObject()) {
        qDebug() << "Not a JSON object";
        return;
    }

    QJsonObject obj = doc.object();

    if (obj.contains("device_id")) {
        QString deviceId = obj["device_id"].toString();
        qDebug() << "Found device:" << deviceId;

        if (!m_discoveredDevices.contains(deviceId)) {
            m_discoveredDevices.insert(deviceId);
            emit discoveredDevicesChanged();

            if (!m_deviceModel->contains(deviceId)) {
                m_deviceModel->addDevice(deviceId);
            }

            emit deviceDiscovered(deviceId);
        }
    }
}

void PNDDeviceConfigurator::processStatusMessage(const QString &deviceId, const QByteArray &payload)
{
    QJsonDocument doc = QJsonDocument::fromJson(payload);
    if (!doc.isObject()) {
        return;
    }

    QJsonObject obj = doc.object();

    // Update device state based on status
    if (obj.contains("state")) {
        QString stateStr = obj["state"].toString();
        if (stateStr == "connected" || stateStr == "on") {
            updateDeviceState(deviceId, PNDDeviceState::CONNECTED);
            emit deviceConnected(deviceId);
        } else if (stateStr == "disconnected" || stateStr == "off") {
            updateDeviceState(deviceId, PNDDeviceState::DISCONNECTED);
            emit deviceDisconnected(deviceId);
        }
    }

    emit deviceStatusUpdated(deviceId, obj);
}

void PNDDeviceConfigurator::processSensorsMessage(const QString &deviceId, const QByteArray &payload)
{
    QJsonDocument doc = QJsonDocument::fromJson(payload);
    if (!doc.isObject()) {
        return;
    }

    QJsonObject obj = doc.object();
    float temperature = static_cast<float>(obj["temperature"].toDouble(0.0));
    float humidity = static_cast<float>(obj["humidity"].toDouble(0.0));

    PNDDevice *device = m_deviceModel->getDevice(deviceId);
    if (device) {
        device->setTemperature(temperature);
        device->setHumidity(humidity);

        // Update state if needed
        if (device->state() != PNDDeviceState::CONNECTED) {
            updateDeviceState(deviceId, PNDDeviceState::CONNECTED);
            emit deviceConnected(deviceId);
        }
    }

    emit deviceSensorsUpdated(deviceId, temperature, humidity);
}

void PNDDeviceConfigurator::processErrorMessage(const QString &deviceId, const QByteArray &payload)
{
    QString errorMessage = QString::fromUtf8(payload);
    emit errorOccurred(QString("Device %1 error: %2").arg(deviceId, errorMessage));
}

void PNDDeviceConfigurator::processAvailabilityMessage(const QString &deviceId, const QByteArray &payload)
{
    QString availability = QString::fromUtf8(payload).trimmed().toLower();
    qDebug() << "Availability for" << deviceId << ":" << availability;

    if (availability == "online") {
        updateDeviceState(deviceId, PNDDeviceState::CONNECTED);
        emit deviceConnected(deviceId);

        // Update last seen
        m_lastSeen[deviceId] = QDateTime::currentDateTime();

    } else if (availability == "offline") {
        qDebug() << "Device" << deviceId << "went offline (LWT or normal disconnect)";
        updateDeviceState(deviceId, PNDDeviceState::DISCONNECTED);
        emit deviceDisconnected(deviceId);

        // Remove from last seen tracking
        m_lastSeen.remove(deviceId);
    }
}

void PNDDeviceConfigurator::updateDeviceState(const QString &deviceId, PNDDeviceState::State state)
{
    PNDDevice *device = m_deviceModel->getDevice(deviceId);
    if (device) {
        device->setState(state);
    }
}

void PNDDeviceConfigurator::publishCommand(const QString &deviceId, const QString &command, const QJsonObject &payload)
{
    if (!m_mqttClient->isConnected()) {
        emit errorOccurred("Cannot send command: Not connected to broker");
        return;
    }

    QJsonObject commandObj = payload;
    if (commandObj.isEmpty()) {
        commandObj["command"] = command;
    }
    commandObj["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);

    QString topic = PNDTopics::deviceCommandTopic(deviceId);
    m_mqttClient->publish(topic, QJsonDocument(commandObj).toJson());

    // Track pending command
    PendingCommand pending;
    pending.command = command;
    pending.timestamp = QDateTime::currentDateTime();
    pending.data = payload;
    m_pendingCommands.insert(deviceId, pending);
}
