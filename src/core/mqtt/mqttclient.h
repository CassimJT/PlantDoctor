#ifndef MQTTCLIENT_H
#define MQTTCLIENT_H

#include <QObject>
#include <QMqttClient>

class MQTTClient : public QObject
{
    Q_OBJECT
public:
    explicit MQTTClient(QObject *parent = nullptr);

signals:
};

#endif // MQTTCLIENT_H
