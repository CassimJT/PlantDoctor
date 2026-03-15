#ifndef PNDTOPICS_H
#define PNDTOPICS_H

#include <QObject>
#include <QString>

class PNDTopics : public QObject
{
    Q_OBJECT

public:
    static const QString ROOT;
    static const QString STATUS;
    static const QString SENSORS;
    static const QString ERROR_TOPIC;
    static const QString COMMAND;
    static const QString POWER;
    static const QString DISCOVERY;
    static const QString CONFIG;
    static const QString AVAILABILITY;

    static QString deviceTopic(const QString &deviceId, const QString &suffix = QString());
    static QString deviceStatusTopic(const QString &deviceId);
    static QString deviceSensorsTopic(const QString &deviceId);
    static QString deviceCommandTopic(const QString &deviceId);
    static QString devicePowerTopic(const QString &deviceId);
    static QString deviceConfigTopic(const QString &deviceId);
    static QString deviceAvailabilityTopic(const QString &deviceId);
    static QString discoveryTopic();

    static bool isDeviceTopic(const QString &topic, QString *deviceId = nullptr);
    static QString extractDeviceId(const QString &topic);

private:
    explicit PNDTopics(QObject *parent = nullptr);
};

#endif // PNDTOPICS_H
