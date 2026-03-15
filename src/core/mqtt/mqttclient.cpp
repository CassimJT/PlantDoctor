#include "mqttclient.h"
#include <QMqttTopicName>
#include <QDebug>

MQTTClient::MQTTClient(QObject *parent)
    : QObject(parent)
    , m_host("192.168.8.130")
    , m_port(1883)
    , m_clientId("PlantDoctor_" + QString::number(QRandomGenerator::global()->generate()))
    , m_isConnected(false)
    , m_client(nullptr)
    , m_pingTimer(nullptr)
{
    setupClient();
}

MQTTClient::~MQTTClient()
{
    if (m_client && m_client->state() == QMqttClient::Connected) {
        m_client->disconnectFromHost();
    }
}

void MQTTClient::setupClient()
{
    if (!m_client) {
        m_client = new QMqttClient(this);

        connect(m_client, &QMqttClient::connected, this, &MQTTClient::onMqttConnected);
        connect(m_client, &QMqttClient::disconnected, this, &MQTTClient::onMqttDisconnected);
        connect(m_client, &QMqttClient::errorChanged, this, &MQTTClient::onMqttError);
        connect(m_client, &QMqttClient::messageReceived, this, &MQTTClient::onMqttMessageReceived);

        m_pingTimer = new QTimer(this);
        m_pingTimer->setInterval(30000); // 30 seconds
        connect(m_pingTimer, &QTimer::timeout, this, &MQTTClient::onPingTimeout);
    }

    m_client->setHostname(m_host);
    m_client->setPort(m_port);
    m_client->setClientId(m_clientId);

    if (!m_username.isEmpty()) {
        m_client->setUsername(m_username);
        m_client->setPassword(m_password);
    }
}

// Properties
QString MQTTClient::host() const { return m_host; }
void MQTTClient::setHost(const QString &host)
{
    if (m_host != host) {
        m_host = host;
        if (m_client) m_client->setHostname(host);
        emit hostChanged();
    }
}

int MQTTClient::port() const { return m_port; }
void MQTTClient::setPort(int port)
{
    if (m_port != port) {
        m_port = port;
        if (m_client) m_client->setPort(port);
        emit portChanged();
    }
}

QString MQTTClient::clientId() const { return m_clientId; }
void MQTTClient::setClientId(const QString &clientId)
{
    if (m_clientId != clientId) {
        m_clientId = clientId;
        if (m_client) m_client->setClientId(clientId);
        emit clientIdChanged();
    }
}

QString MQTTClient::username() const { return m_username; }
void MQTTClient::setUsername(const QString &username) { m_username = username; }

QString MQTTClient::password() const { return m_password; }
void MQTTClient::setPassword(const QString &password) { m_password = password; }

bool MQTTClient::isConnected() const { return m_isConnected; }

void MQTTClient::setConnected(bool state)
{
    if (m_isConnected != state) {
        m_isConnected = state;
        emit connectionStateChanged();

        if (state) {
            m_pingTimer->start();
            emit connected();
        } else {
            m_pingTimer->stop();
            emit disconnected();
        }
    }
}

// Public slots
void MQTTClient::connectToBroker()
{
    if (!m_client) {
        setupClient();
    }

    if (m_client->state() == QMqttClient::Connected) {
        return;
    }

    m_client->connectToHost();
}

void MQTTClient::disconnectFromBroker()
{
    if (m_client && m_client->state() == QMqttClient::Connected) {
        m_client->disconnectFromHost();
    }
}

void MQTTClient::publish(const QString &topic, const QByteArray &payload, quint8 qos, bool retain)
{
    if (m_client && m_client->state() == QMqttClient::Connected) {
        m_client->publish(QMqttTopicName(topic), payload, qos, retain);
    } else {
        emit errorOccurred("Cannot publish: Not connected to broker");
    }
}

void MQTTClient::subscribe(const QString &topic, quint8 qos)
{
    if (!m_client || m_client->state() != QMqttClient::Connected) {
        emit errorOccurred("Cannot subscribe: Not connected to broker");
        return;
    }

    if (m_subscriptions.contains(topic)) {
        return; // Already subscribed
    }

    auto sub = m_client->subscribe(QMqttTopicFilter(topic), qos);
    if (sub) {
        m_subscriptions[topic] = sub;
        connect(sub, &QMqttSubscription::messageReceived,
                this, &MQTTClient::onSubscriptionMessageReceived);
        connect(sub, &QMqttSubscription::stateChanged,
                this, [this, sub](QMqttSubscription::SubscriptionState state) {
                    if (state == QMqttSubscription::Subscribed) {
                        qDebug() << "Subscribed to:" << sub->topic().filter();
                    }
                });
    }
}

void MQTTClient::unsubscribe(const QString &topic)
{
    if (m_subscriptions.contains(topic)) {
        auto sub = m_subscriptions.take(topic);
        if (sub) {
            sub->unsubscribe();
            sub->deleteLater();
        }
    }
}

// Private slots
void MQTTClient::onMqttConnected()
{
    setConnected(true);

    // Resubscribe to all topics
    for (auto it = m_subscriptions.begin(); it != m_subscriptions.end(); ) {
        QString topic = it.key();
        auto oldSub = it.value();
        quint8 qos = oldSub ? oldSub->qos() : 0;

        it = m_subscriptions.erase(it);
        oldSub->deleteLater();

        subscribe(topic, qos);
    }
}

void MQTTClient::onMqttDisconnected()
{
    setConnected(false);
}

void MQTTClient::onMqttError(QMqttClient::ClientError error)
{
    QString errorMsg;
    switch (error) {
    case QMqttClient::NoError:
        return;
    case QMqttClient::InvalidProtocolVersion:
        errorMsg = "Invalid protocol version";
        break;
    case QMqttClient::IdRejected:
        errorMsg = "Client ID rejected";
        break;
    case QMqttClient::ServerUnavailable:
        errorMsg = "Server unavailable";
        break;
    case QMqttClient::BadUsernameOrPassword:
        errorMsg = "Bad username or password";
        break;
    case QMqttClient::NotAuthorized:
        errorMsg = "Not authorized";
        break;
    case QMqttClient::TransportInvalid:
        errorMsg = "Transport invalid";
        break;
    case QMqttClient::ProtocolViolation:
        errorMsg = "Protocol violation";
        break;
    case QMqttClient::UnknownError:
    default:
        errorMsg = "Unknown error";
        break;
    }

    emit errorOccurred("MQTT Error: " + errorMsg);
}

void MQTTClient::onMqttMessageReceived(const QByteArray &message, const QMqttTopicName &topic)
{
    emit messageReceived(topic.name(), message);
}

void MQTTClient::onSubscriptionMessageReceived(const QMqttMessage &msg)
{
    emit messageReceived(msg.topic().name(), msg.payload());
}

void MQTTClient::onSubscriptionReceived(QMqttSubscription *sub)
{
    if (sub && sub->state() == QMqttSubscription::Subscribed) {
        qDebug() << "Subscribed to:" << sub->topic().filter();
    }
}

void MQTTClient::onPingTimeout()
{
    if (m_client && m_client->state() == QMqttClient::Connected) {
        // Check if we're still connected by pinging
        // The ping is handled automatically by QMqttClient when autoKeepAlive is true
        // which is the default
        if (m_client->state() != QMqttClient::Connected) {
            setConnected(false);
        }
    }
}
