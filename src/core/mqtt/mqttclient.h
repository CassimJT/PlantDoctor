#ifndef MQTTCLIENT_H
#define MQTTCLIENT_H

#include <QObject>
#include <QMqttClient>
#include <QMqttSubscription>
#include <QTimer>
#include <QRandomGenerator>

class MQTTClient : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString host READ host WRITE setHost NOTIFY hostChanged)
    Q_PROPERTY(int port READ port WRITE setPort NOTIFY portChanged)
    Q_PROPERTY(QString clientId READ clientId WRITE setClientId NOTIFY clientIdChanged)
    Q_PROPERTY(bool isConnected READ isConnected NOTIFY connectionStateChanged)

public:
    explicit MQTTClient(QObject *parent = nullptr);
    ~MQTTClient();

    // Properties
    QString host() const;
    void setHost(const QString &host);

    int port() const;
    void setPort(int port);

    QString clientId() const;
    void setClientId(const QString &clientId);

    QString username() const;
    void setUsername(const QString &username);

    QString password() const;
    void setPassword(const QString &password);

    bool isConnected() const;

public slots:
    void connectToBroker();
    void disconnectFromBroker();
    void publish(const QString &topic, const QByteArray &payload, quint8 qos = 0, bool retain = false);
    void subscribe(const QString &topic, quint8 qos = 0);
    void unsubscribe(const QString &topic);

signals:
    void messageReceived(const QString &topic, const QByteArray &payload);
    void connected();
    void disconnected();
    void connectionStateChanged();
    void errorOccurred(const QString &message);

    // Property change signals
    void hostChanged();
    void portChanged();
    void clientIdChanged();

private slots:
    void onMqttConnected();
    void onMqttDisconnected();
    void onMqttError(QMqttClient::ClientError error);
    void onMqttMessageReceived(const QByteArray &message, const QMqttTopicName &topic);
    void onPingTimeout();
    void onSubscriptionReceived(QMqttSubscription *sub);
    void onSubscriptionMessageReceived(const QMqttMessage &msg);

private:
    void setConnected(bool state);
    void setupClient();

    QString m_host;
    int m_port;
    QString m_clientId;
    QString m_username;
    QString m_password;
    bool m_isConnected;

    QMqttClient *m_client;
    QTimer *m_pingTimer;
    QHash<QString, QMqttSubscription*> m_subscriptions;
};

#endif // MQTTCLIENT_H
