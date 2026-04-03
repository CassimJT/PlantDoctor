#ifndef PNDTOPICS_H
#define PNDTOPICS_H

#include <QObject>
#include <QString>

class PNDTopics : public QObject
{
    Q_OBJECT

public:
    // Core topics
    static inline const QString ROOT = "plantdoctor";
    static inline const QString STATUS = "status";
    static inline const QString SENSORS = "sensors";
    static inline const QString ERROR_TOPIC = "error";
    static inline const QString COMMAND = "command";
    static inline const QString POWER = "power";
    static inline const QString DISCOVERY = "discovery";
    static inline const QString CONFIG = "config";
    static inline const QString AVAILABILITY = "availability";

    // Topic builders
    static QString deviceTopic(const QString &deviceId, const QString &suffix = QString());
    static QString deviceStatusTopic(const QString &deviceId);
    static QString deviceSensorsTopic(const QString &deviceId);
    static QString deviceCommandTopic(const QString &deviceId);
    static QString devicePowerTopic(const QString &deviceId);
    static QString deviceConfigTopic(const QString &deviceId);
    static QString deviceAvailabilityTopic(const QString &deviceId);
    static QString discoveryTopic();

    // Topic parsing
    static bool isDeviceTopic(const QString &topic, QString *deviceId = nullptr);
    static QString extractDeviceId(const QString &topic);

private:
    PNDTopics() = delete; // prevent instantiation
};

#endif // PNDTOPICS_H