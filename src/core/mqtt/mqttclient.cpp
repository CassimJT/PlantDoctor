#include "mqttclient.h"
#include <QRandomGenerator>
#include <QDebug>

MQTTClient::MQTTClient(QObject *parent)
    : QObject(parent),
    m_client(new QMqttClient(this)),
    m_host("127.0.0.1"),
    m_port(1883),
    m_clientId("PlantDoctor_" + QString::number(QRandomGenerator::global()->generate())),
    m_isConnected(false)
{
    setupClient();
}

MQTTClient::~MQTTClient()
{
    if (m_client->state() == QMqttClient::Connected) {
        m_client->disconnectFromHost();
    }
}

void MQTTClient::setupClient()
{
    m_client->setHostname(m_host);
    m_client->setPort(m_port);
    m_client->setClientId(m_clientId);

    if (!m_username.isEmpty()) {
        m_client->setUsername(m_username);
        m_client->setPassword(m_password);
    }

    connect(m_client, &QMqttClient::stateChanged,
            this, &MQTTClient::onStateChanged);

    connect(m_client, &QMqttClient::messageReceived,
            this, &MQTTClient::onMessageReceived);

    connect(m_client, &QMqttClient::errorChanged,
            this, &MQTTClient::onErrorChanged);
}


// ==============================
// Properties
// ==============================

QString MQTTClient::host() const { return m_host; }

void MQTTClient::setHost(const QString &host)
{
    if (m_host == host) return;
    m_host = host;
    m_client->setHostname(host);
    emit hostChanged();
}

int MQTTClient::port() const { return m_port; }

void MQTTClient::setPort(int port)
{
    if (m_port == port) return;
    m_port = port;
    m_client->setPort(port);
    emit portChanged();
}

QString MQTTClient::clientId() const { return m_clientId; }

void MQTTClient::setClientId(const QString &clientId)
{
    if (m_clientId == clientId) return;
    m_clientId = clientId;
    m_client->setClientId(clientId);
    emit clientIdChanged();
}

QString MQTTClient::username() const { return m_username; }
void MQTTClient::setUsername(const QString &username)
{
    m_username = username;
}

QString MQTTClient::password() const { return m_password; }
void MQTTClient::setPassword(const QString &password)
{
    m_password = password;
}

bool MQTTClient::isConnected() const
{
    return m_isConnected;
}


// ==============================
// Connection control
// ==============================

void MQTTClient::connectToBroker()
{
    if (m_client->state() == QMqttClient::Connected)
        return;

    setupClient(); // ensure latest config applied
    m_client->connectToHost();
}

void MQTTClient::disconnectFromBroker()
{
    if (m_client->state() == QMqttClient::Connected) {
        m_client->disconnectFromHost();
    }
}


// ==============================
// Publish / Subscribe
// ==============================

void MQTTClient::publish(const QString &topic, const QByteArray &payload,
                         quint8 qos, bool retain)
{
    if (m_client->state() != QMqttClient::Connected) {
        emit errorOccurred("Publish failed: not connected");
        return;
    }

    m_client->publish(topic, payload, qos, retain);
}

void MQTTClient::subscribe(const QString &topic, quint8 qos)
{
    // Store ALWAYS (even if offline)
    m_topicRegistry[topic] = qos;

    if (m_client->state() != QMqttClient::Connected)
        return;

    m_client->subscribe(topic, qos);
}

void MQTTClient::unsubscribe(const QString &topic)
{
    m_topicRegistry.remove(topic);

    if (m_client->state() == QMqttClient::Connected) {
        m_client->unsubscribe(topic);
    }
}


// ==============================
// Internal logic
// ==============================

void MQTTClient::onStateChanged(QMqttClient::ClientState state)
{
    bool nowConnected = (state == QMqttClient::Connected);
    setConnected(nowConnected);

    if (nowConnected) {
        resubscribeAll();
    }
}

void MQTTClient::resubscribeAll()
{
    for (auto it = m_topicRegistry.begin(); it != m_topicRegistry.end(); ++it) {
        m_client->subscribe(it.key(), it.value());
    }
}

void MQTTClient::setConnected(bool state)
{
    if (m_isConnected == state)
        return;

    m_isConnected = state;
    emit connectionStateChanged();

    if (state)
        emit connected();
    else
        emit disconnected();
}

void MQTTClient::onMessageReceived(const QByteArray &message,
                                   const QMqttTopicName &topic)
{
    emit messageReceived(topic.name(), message);
}

void MQTTClient::onErrorChanged(QMqttClient::ClientError error)
{
    if (error == QMqttClient::NoError)
        return;

    emit errorOccurred(QString("MQTT Error: %1").arg(error));
}