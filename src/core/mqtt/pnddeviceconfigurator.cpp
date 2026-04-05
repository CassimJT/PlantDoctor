#include "pnddeviceconfigurator.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

PNDDeviceConfigurator::PNDDeviceConfigurator(QObject *parent)
    : QObject(parent)
    , m_mqttClient(new MQTTClient(this))
    , m_deviceModel(new PNDDeviceModel(this))
    , m_scanTimer(new QTimer(this))
    , m_availabilityTimer(new QTimer(this))
    , m_isScanning(false)
    , m_availabilityTimeout(120)
{
    qDebug() << "==================================================";
    qDebug() << "PNDDeviceConfigurator initialized";
    qDebug() << "Device model created:" << m_deviceModel;
    qDebug() << "==================================================";

    connect(m_mqttClient, &MQTTClient::connected,
            this, &PNDDeviceConfigurator::handleMqttConnected);
    connect(m_mqttClient, &MQTTClient::disconnected,
            this, &PNDDeviceConfigurator::handleMqttDisconnected);
    connect(m_mqttClient, &MQTTClient::messageReceived,
            this, &PNDDeviceConfigurator::handleMqttMessage);
    connect(m_mqttClient, &MQTTClient::errorOccurred,
            this, &PNDDeviceConfigurator::handleMqttError);

    m_scanTimer->setSingleShot(true);
    connect(m_scanTimer, &QTimer::timeout, this, &PNDDeviceConfigurator::onScanTimeout);

    m_availabilityTimer->setInterval(30000);
    connect(m_availabilityTimer, &QTimer::timeout,
            this, &PNDDeviceConfigurator::checkDeviceAvailability);
}

PNDDeviceConfigurator::~PNDDeviceConfigurator()
{
    stopScan();
    disconnectFromBroker();
}

// =====================
// Properties
// =====================

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

bool PNDDeviceConfigurator::isBrokerConnected() const
{
    return m_mqttClient->isConnected();
}

// =====================
// MQTT Configuration
// =====================

void PNDDeviceConfigurator::setMqttBroker(const QString &host, int port)
{
    m_mqttClient->setHost(host);
    m_mqttClient->setPort(port);
    qDebug() << "MQTT broker set to:" << host << ":" << port;
}

void PNDDeviceConfigurator::setMqttCredentials(const QString &username, const QString &password)
{
    m_mqttClient->setUsername(username);
    m_mqttClient->setPassword(password);
    qDebug() << "MQTT credentials set for user:" << username;
}

// =====================
// Public Slots
// =====================

void PNDDeviceConfigurator::connectToBroker()
{
    qDebug() << "connectToBroker called";
    m_mqttClient->connectToBroker();
}

void PNDDeviceConfigurator::disconnectFromBroker()
{
    qDebug() << "disconnectFromBroker called";
    m_availabilityTimer->stop();
    m_mqttClient->disconnectFromBroker();
}

void PNDDeviceConfigurator::scanForDevices(int timeoutSeconds)
{
    qDebug() << "scanForDevices called with timeout:" << timeoutSeconds << "s";

    if (m_isScanning) {
        qDebug() << "Already scanning, ignoring request";
        return;
    }

    m_isScanning = true;
    m_discoveredDevices.clear();
    emit scanningChanged();
    emit discoveredDevicesChanged();

    if (m_mqttClient->isConnected()) {
        qDebug() << "MQTT connected, subscribing to discovery topic";

        m_mqttClient->subscribe(PNDTopics::discoveryTopic());

        QJsonObject request;
        request["command"] = "discover";
        request["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);

        QByteArray payload = QJsonDocument(request).toJson();
        qDebug() << "Publishing discovery request:" << request;

        m_mqttClient->publish(PNDTopics::discoveryTopic(), payload);
    } else {
        qDebug() << "MQTT not connected, cannot publish discovery request";
    }

    m_scanTimer->start(timeoutSeconds * 1000);
}

void PNDDeviceConfigurator::stopScan()
{
    qDebug() << "stopScan called";

    if (m_isScanning) {
        m_scanTimer->stop();
        m_isScanning = false;
        emit scanningChanged();
        qDebug() << "Scan stopped";
    }
}

void PNDDeviceConfigurator::connectToDevice(const QString &deviceId)
{
    qDebug() << "connectToDevice called for device:" << deviceId;

    if (!m_deviceModel->contains(deviceId)) {
        qDebug() << "Device" << deviceId << "not in model, adding it";
        m_deviceModel->addDeviceById(deviceId);
    } else {
        qDebug() << "Device" << deviceId << "already in model";
    }

    PNDDevice* device = m_deviceModel->getDevice(deviceId);
    if (device) {
        qDebug() << "Device found in model, current state:" << device->state();
        updateDeviceState(deviceId, PNDDeviceState::CONNECTING);

        qDebug() << "Subscribing to device topics for" << deviceId;
        m_mqttClient->subscribe(PNDTopics::deviceStatusTopic(deviceId));
        m_mqttClient->subscribe(PNDTopics::deviceSensorsTopic(deviceId));
        m_mqttClient->subscribe(PNDTopics::deviceAvailabilityTopic(deviceId));
        m_mqttClient->subscribe(PNDTopics::deviceConfigTopic(deviceId));

        qDebug() << "Requesting status for" << deviceId;
        requestDeviceStatus(deviceId);
    } else {
        qDebug() << "ERROR: Device" << deviceId << "not found in model after adding!";
    }
}

void PNDDeviceConfigurator::disconnectDevice(const QString &deviceId)
{
    qDebug() << "disconnectDevice called for device:" << deviceId;

    PNDDevice* device = m_deviceModel->getDevice(deviceId);
    if (device) {
        qDebug() << "Device found, current state:" << device->state();
        updateDeviceState(deviceId, PNDDeviceState::DISCONNECTED);

        qDebug() << "Unsubscribing from device topics for" << deviceId;
        m_mqttClient->unsubscribe(PNDTopics::deviceStatusTopic(deviceId));
        m_mqttClient->unsubscribe(PNDTopics::deviceSensorsTopic(deviceId));
        m_mqttClient->unsubscribe(PNDTopics::deviceAvailabilityTopic(deviceId));
        m_mqttClient->unsubscribe(PNDTopics::deviceConfigTopic(deviceId));

        if (m_lastSeen.contains(deviceId)) {
            m_lastSeen.remove(deviceId);
            qDebug() << "Removed" << deviceId << "from last_seen tracking";
        }
    } else {
        qDebug() << "Device" << deviceId << "not found in model";
    }
}

void PNDDeviceConfigurator::setDevicePower(const QString &deviceId, bool powerOn)
{
    qDebug() << "setDevicePower called for" << deviceId << ": power_on=" << powerOn;

    if (!m_mqttClient->isConnected()) {
        QString errorMsg = "MQTT client not connected";
        qDebug() << "ERROR:" << errorMsg;
        emit errorOccurred(errorMsg);
        return;
    }

    QJsonObject command;
    command["command"] = "power";
    command["value"] = powerOn;
    command["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);

    QString topic = PNDTopics::deviceCommandTopic(deviceId);
    QByteArray payload = QJsonDocument(command).toJson();
    qDebug() << "Publishing power command to" << topic << ":" << command;

    m_mqttClient->publish(topic, payload);

    PendingCommand pending;
    pending.command = "power";
    pending.timestamp = QDateTime::currentDateTime();
    pending.data["power_on"] = powerOn;
    m_pendingCommands[deviceId] = pending;

    qDebug() << "Pending command tracked for" << deviceId;
}

void PNDDeviceConfigurator::configureDevice(const QString &deviceId, const QJsonObject &config)
{
    qDebug() << "configureDevice called for" << deviceId << ":" << config;

    if (!m_mqttClient->isConnected()) {
        QString errorMsg = "MQTT client not connected";
        qDebug() << "ERROR:" << errorMsg;
        emit errorOccurred(errorMsg);
        return;
    }

    QJsonObject command;
    command["command"] = "configure";
    command["config"] = config;
    command["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);

    QString topic = PNDTopics::deviceCommandTopic(deviceId);
    QByteArray payload = QJsonDocument(command).toJson();
    qDebug() << "Publishing configure command to" << topic << ":" << command;

    m_mqttClient->publish(topic, payload);
}

void PNDDeviceConfigurator::requestDeviceStatus(const QString &deviceId)
{
    qDebug() << "requestDeviceStatus called for" << deviceId;

    if (!m_mqttClient->isConnected()) {
        qDebug() << "MQTT not connected, cannot request status";
        return;
    }

    QJsonObject command;
    command["command"] = "get_status";
    command["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);

    QString topic = PNDTopics::deviceCommandTopic(deviceId);
    QByteArray payload = QJsonDocument(command).toJson();
    qDebug() << "Publishing status request to" << topic << ":" << command;

    m_mqttClient->publish(topic, payload);
}

void PNDDeviceConfigurator::requestDeviceSensors(const QString &deviceId)
{
    qDebug() << "requestDeviceSensors called for" << deviceId;

    if (!m_mqttClient->isConnected()) {
        qDebug() << "MQTT not connected, cannot request sensors";
        return;
    }

    QJsonObject command;
    command["command"] = "get_sensors";
    command["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);

    QString topic = PNDTopics::deviceCommandTopic(deviceId);
    QByteArray payload = QJsonDocument(command).toJson();
    qDebug() << "Publishing sensors request to" << topic << ":" << command;

    m_mqttClient->publish(topic, payload);
}

// =====================
// Private Slots
// =====================

void PNDDeviceConfigurator::handleMqttMessage(const QString &topic, const QByteArray &payload)
{
    qDebug() << "\n--- MQTT Message Received ---";
    qDebug() << "Topic:" << topic;
    qDebug() << "Payload:" << payload;

    QString deviceId = PNDTopics::extractDeviceId(topic);
    bool isDeviceTopic = !deviceId.isEmpty();

    qDebug() << "is_device_topic:" << isDeviceTopic << ", device_id:" << deviceId;

    if (isDeviceTopic && !deviceId.isEmpty()) {
        m_lastSeen[deviceId] = QDateTime::currentDateTime();
        qDebug() << "Updated last_seen for" << deviceId;
    }

    if (topic == PNDTopics::discoveryTopic()) {
        qDebug() << "Processing discovery message";
        processDiscoveryMessage(payload);
    }
    else if (isDeviceTopic && !deviceId.isEmpty()) {
        if (topic.endsWith(PNDTopics::STATUS)) {
            qDebug() << "Processing status message for" << deviceId;
            processStatusMessage(deviceId, payload);
        }
        else if (topic.endsWith(PNDTopics::SENSORS)) {
            qDebug() << "Processing sensors message for" << deviceId;
            processSensorsMessage(deviceId, payload);
        }
        else if (topic.endsWith(PNDTopics::ERROR_TOPIC)) {
            qDebug() << "Processing error message for" << deviceId;
            processErrorMessage(deviceId, payload);
        }
        else if (topic.endsWith(PNDTopics::AVAILABILITY)) {
            qDebug() << "Processing availability message for" << deviceId;
            processAvailabilityMessage(deviceId, payload);
        }
    }
}

void PNDDeviceConfigurator::handleMqttConnected()
{
    qDebug() << "\n*** MQTT Connected ***";

    qDebug() << "Subscribing to global topics...";
    m_mqttClient->subscribe(PNDTopics::discoveryTopic());
    m_mqttClient->subscribe("plantdoctor/device/+/availability");

    m_availabilityTimer->start();
    qDebug() << "Availability timer started";

    int activeCount = 0;
    for (int i = 0; i < m_deviceModel->rowCount(); ++i) {
        PNDDevice* device = m_deviceModel->getDevice(i);
        if (device && device->state() != PNDDeviceState::DISCONNECTED) {
            activeCount++;
            qDebug() << "Resubscribing to active device:" << device->deviceId();
            connectToDevice(device->deviceId());
        }
    }

    qDebug() << "Resubscribed to" << activeCount << "active devices";

    emit brokerConnectionChanged();
    qDebug() << "brokerConnectionChanged signal emitted";
}

void PNDDeviceConfigurator::handleMqttDisconnected()
{
    qDebug() << "\n*** MQTT Disconnected ***";

    m_availabilityTimer->stop();
    qDebug() << "Availability timer stopped";

    int disconnectedCount = 0;
    for (int i = 0; i < m_deviceModel->rowCount(); ++i) {
        PNDDevice* device = m_deviceModel->getDevice(i);
        if (device && device->state() == PNDDeviceState::CONNECTED) {
            disconnectedCount++;
            qDebug() << "Marking device as disconnected:" << device->deviceId();
            updateDeviceState(device->deviceId(), PNDDeviceState::DISCONNECTED);
        }
    }

    qDebug() << "Marked" << disconnectedCount << "devices as disconnected";
    m_lastSeen.clear();
    qDebug() << "last_seen cleared";

    emit brokerConnectionChanged();
    emit brokerDisconnected();
    qDebug() << "Signals emitted: brokerConnectionChanged, brokerDisconnected";
}

void PNDDeviceConfigurator::handleMqttError(const QString &error)
{
    qDebug() << "\n*** MQTT Error:" << error << "***";
    emit errorOccurred("MQTT Error: " + error);
}

void PNDDeviceConfigurator::onScanTimeout()
{
    qDebug() << "\n*** Scan timeout ***";
    stopScan();
}

void PNDDeviceConfigurator::checkDeviceAvailability()
{
    QDateTime now = QDateTime::currentDateTime();
    qDebug() << "\n--- Checking device availability ---";
    qDebug() << "Tracked devices:" << m_lastSeen.size();
    qDebug() << "Current time:" << now.toString();

    QList<QString> toRemove;

    for (auto it = m_lastSeen.begin(); it != m_lastSeen.end(); ++it) {
        QString deviceId = it.key();
        QDateTime lastSeen = it.value();
        qint64 secondsSince = lastSeen.secsTo(now);

        qDebug() << "Device" << deviceId << ": last seen" << secondsSince << "seconds ago";

        PNDDevice* device = m_deviceModel->getDevice(deviceId);
        if (!device) {
            qDebug() << "Device" << deviceId << "no longer in model, removing from tracking";
            toRemove.append(deviceId);
            continue;
        }

        if (device->state() == PNDDeviceState::CONNECTED) {
            if (secondsSince > m_availabilityTimeout) {
                qDebug() << "Device" << deviceId << "not seen for" << secondsSince
                         << "seconds (> " << m_availabilityTimeout << ")";
                qDebug() << "Marking device as disconnected";
                updateDeviceState(deviceId, PNDDeviceState::DISCONNECTED);
                emit deviceDisconnected(deviceId);
                toRemove.append(deviceId);
            }
        }
    }

    for (const QString& deviceId : toRemove) {
        m_lastSeen.remove(deviceId);
    }
}

// =====================
// Private Methods
// =====================

void PNDDeviceConfigurator::processDiscoveryMessage(const QByteArray &payload)
{
    qDebug() << "\n--- Processing discovery message ---";

    QJsonDocument doc = QJsonDocument::fromJson(payload);
    if (!doc.isObject()) {
        qDebug() << "Invalid JSON payload";
        return;
    }

    QJsonObject data = doc.object();
    qDebug() << "Discovery data:" << data;

    if (data.contains("device_id")) {
        QString deviceId = data["device_id"].toString();
        qDebug() << "Found device ID:" << deviceId;

        bool existingDevice = m_deviceModel->contains(deviceId);

        if (!m_discoveredDevices.contains(deviceId)) {
            qDebug() << "New device discovered:" << deviceId;
            m_discoveredDevices.insert(deviceId);
            emit discoveredDevicesChanged();
        }

        if (!m_deviceModel->contains(deviceId)) {
            qDebug() << "Adding device" << deviceId << "to model";
            m_deviceModel->addDeviceById(deviceId);

            if (m_deviceModel->contains(deviceId)) {
                qDebug() << "Device" << deviceId << "successfully added to model";
                qDebug() << "Model now has" << m_deviceModel->rowCount() << "devices";
            } else {
                qDebug() << "ERROR: Device" << deviceId << "was NOT added to model!";
            }
        } else {
            qDebug() << "Device" << deviceId << "already in model";
        }

        if (!m_discoveredDevices.contains(deviceId) || !existingDevice) {
            emit deviceDiscovered(deviceId);
            qDebug() << "deviceDiscovered signal emitted for" << deviceId;
        }
    } else {
        qDebug() << "No device_id in discovery message";
    }
}

void PNDDeviceConfigurator::processStatusMessage(const QString &deviceId, const QByteArray &payload)
{
    qDebug() << "\n--- Processing status message for" << deviceId << "---";

    QJsonDocument doc = QJsonDocument::fromJson(payload);
    if (!doc.isObject()) {
        qDebug() << "Invalid JSON payload";
        return;
    }

    QJsonObject data = doc.object();
    qDebug() << "Status data:" << data;

    if (data.contains("state")) {
        QString stateStr = data["state"].toString();
        qDebug() << "State string:" << stateStr;

        if (stateStr == "connected" || stateStr == "on") {
            qDebug() << "Device" << deviceId << "is connected";
            updateDeviceState(deviceId, PNDDeviceState::CONNECTED);
            emit deviceConnected(deviceId);
        } else if (stateStr == "disconnected" || stateStr == "off") {
            qDebug() << "Device" << deviceId << "is disconnected";
            updateDeviceState(deviceId, PNDDeviceState::DISCONNECTED);
            emit deviceDisconnected(deviceId);
        }
    }

    emit deviceStatusUpdated(deviceId, data);
    qDebug() << "deviceStatusUpdated signal emitted for" << deviceId;
}

void PNDDeviceConfigurator::processSensorsMessage(const QString &deviceId, const QByteArray &payload)
{
    qDebug() << "\n--- Processing sensors message for" << deviceId << "---";

    // CRITICAL: Auto-add device if not in model
    if (!m_deviceModel->contains(deviceId)) {
        qDebug() << "Device not in model, auto-adding from sensors message:" << deviceId;
        m_deviceModel->addDeviceById(deviceId);
    }

    QJsonDocument doc = QJsonDocument::fromJson(payload);
    if (!doc.isObject()) {
        qDebug() << "Invalid JSON payload";
        return;
    }

    QJsonObject data = doc.object();
    qDebug() << "Sensors data:" << data;

    float temperature = data.value("temperature").toDouble(0.0);
    float humidity = data.value("humidity").toDouble(0.0);
    qDebug() << "Temperature:" << temperature << ", Humidity:" << humidity;

    PNDDevice* device = m_deviceModel->getDevice(deviceId);
    if (device) {
        qDebug() << "Device found in model, updating sensor values";
        device->setTemperature(temperature);
        device->setHumidity(humidity);

        if (device->state() != PNDDeviceState::CONNECTED) {
            qDebug() << "Device state was" << device->state() << ", updating to CONNECTED";
            updateDeviceState(deviceId, PNDDeviceState::CONNECTED);
            emit deviceConnected(deviceId);
        }
    } else {
        qDebug() << "ERROR: Device" << deviceId << "not found in model after adding!";
    }

    emit deviceSensorsUpdated(deviceId, temperature, humidity);
    qDebug() << "deviceSensorsUpdated signal emitted for" << deviceId;
}

void PNDDeviceConfigurator::processErrorMessage(const QString &deviceId, const QByteArray &payload)
{
    QString errorMsg = QString::fromUtf8(payload);
    qDebug() << "\n--- Error message from" << deviceId << ":" << errorMsg << "---";
    emit errorOccurred(QString("Device %1 error: %2").arg(deviceId, errorMsg));
}

void PNDDeviceConfigurator::processAvailabilityMessage(const QString &deviceId, const QByteArray &payload)
{
    QString availability = QString::fromUtf8(payload).trimmed().toLower();
    qDebug() << "\n--- Availability message for" << deviceId << ":" << availability << "---";

    // Auto-add device when it comes online
    if (availability == "online") {
        if (!m_deviceModel->contains(deviceId)) {
            qDebug() << "Device not in model, auto-adding from availability:" << deviceId;
            m_deviceModel->addDeviceById(deviceId);
        }
    }

    PNDDevice* device = m_deviceModel->getDevice(deviceId);

    if (availability == "online") {
        qDebug() << "Device" << deviceId << "is online";

        if (device) {
            qDebug() << "Subscribing to device topics for" << deviceId;

            m_mqttClient->subscribe(PNDTopics::deviceSensorsTopic(deviceId));
            m_mqttClient->subscribe(PNDTopics::deviceStatusTopic(deviceId));
            m_mqttClient->subscribe(PNDTopics::deviceConfigTopic(deviceId));

            qDebug() << "Subscribed to:";
            qDebug() << "  " << PNDTopics::deviceSensorsTopic(deviceId);
            qDebug() << "  " << PNDTopics::deviceStatusTopic(deviceId);
            qDebug() << "  " << PNDTopics::deviceConfigTopic(deviceId);

            updateDeviceState(deviceId, PNDDeviceState::CONNECTED);
            emit deviceConnected(deviceId);

            m_lastSeen[deviceId] = QDateTime::currentDateTime();
            qDebug() << "Added" << deviceId << "to last_seen tracking";
        } else {
            qDebug() << "ERROR: Failed to create device" << deviceId;
        }
    }
    else if (availability == "offline") {
        qDebug() << "Device" << deviceId << "went offline";

        if (device) {
            updateDeviceState(deviceId, PNDDeviceState::DISCONNECTED);
            emit deviceDisconnected(deviceId);
        }

        if (m_lastSeen.contains(deviceId)) {
            m_lastSeen.remove(deviceId);
        }
    }
}

void PNDDeviceConfigurator::updateDeviceState(const QString &deviceId, PNDDeviceState::State state)
{
    qDebug() << "\n--- Updating device state for" << deviceId << "to" << static_cast<int>(state) << "---";

    PNDDevice* device = m_deviceModel->getDevice(deviceId);
    if (device) {
        int oldState = device->state();
        device->setState(state);
        qDebug() << "Device state changed from" << oldState << "to" << static_cast<int>(state);

        int currentState = device->state();
        qDebug() << "Verified current state:" << currentState;
    } else {
        qDebug() << "ERROR: Device" << deviceId << "not found in model!";
    }
}

void PNDDeviceConfigurator::publishCommand(const QString &deviceId,
                                           const QString &command,
                                           const QJsonObject &payload)
{
    if (!m_mqttClient->isConnected()) {
        emit errorOccurred("Not connected");
        return;
    }

    QJsonObject obj = payload;
    obj["command"] = command;

    QString topic = PNDTopics::deviceCommandTopic(deviceId);
    m_mqttClient->publish(topic, QJsonDocument(obj).toJson());

    PendingCommand pending;
    pending.command = command;
    pending.timestamp = QDateTime::currentDateTime();
    pending.data = payload;
    m_pendingCommands[deviceId] = pending;
}