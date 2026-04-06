#ifndef MQTTCLIENT_H
#define MQTTCLIENT_H

#include <QObject>
#include <QMqttClient>
#include <QMqttSubscription>
#include <QTimer>
#include <QHash>

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

    void publish(const QString &topic, const QByteArray &payload,
                 quint8 qos = 0, bool retain = false);

    void subscribe(const QString &topic, quint8 qos = 0);
    void unsubscribe(const QString &topic);

signals:
    void messageReceived(const QString &topic, const QByteArray &payload);

    void connected();
    void disconnected();
    void connectionStateChanged();

    void errorOccurred(const QString &message);

    void hostChanged();
    void portChanged();
    void clientIdChanged();

private slots:
    void onStateChanged(QMqttClient::ClientState state);
    void onMessageReceived(const QByteArray &message, const QMqttTopicName &topic);
    void onErrorChanged(QMqttClient::ClientError error);

private:
    void setupClient();
    void resubscribeAll();
    void setConnected(bool state);

private:
    QMqttClient *m_client;

    QString m_host;
    int m_port;
    QString m_clientId;
    QString m_username;
    QString m_password;

    bool m_isConnected;

    // persistent topic registry (Python-equivalent)
    QHash<QString, quint8> m_topicRegistry;
};

#endif // MQTTCLIENT_H