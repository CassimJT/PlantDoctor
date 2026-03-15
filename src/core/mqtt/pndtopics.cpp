#include "pndtopics.h"
#include <QRegularExpression>

const QString PNDTopics::ROOT = "plantdoctor";
const QString PNDTopics::STATUS = "status";
const QString PNDTopics::SENSORS = "sensors";
const QString PNDTopics::ERROR_TOPIC = "error";
const QString PNDTopics::COMMAND = "command";
const QString PNDTopics::POWER = "power";
const QString PNDTopics::DISCOVERY = "discovery";
const QString PNDTopics::CONFIG = "config";
const QString PNDTopics::AVAILABILITY = "availability";

QString PNDTopics::deviceTopic(const QString &deviceId, const QString &suffix)
{
    QString topic = QString("%1/device/%2").arg(ROOT, deviceId);
    if (!suffix.isEmpty()) {
        topic += "/" + suffix;
    }
    return topic;
}

QString PNDTopics::deviceStatusTopic(const QString &deviceId)
{
    return deviceTopic(deviceId, STATUS);
}

QString PNDTopics::deviceSensorsTopic(const QString &deviceId)
{
    return deviceTopic(deviceId, SENSORS);
}

QString PNDTopics::deviceCommandTopic(const QString &deviceId)
{
    return deviceTopic(deviceId, COMMAND);
}

QString PNDTopics::devicePowerTopic(const QString &deviceId)
{
    return deviceTopic(deviceId, POWER);
}

QString PNDTopics::deviceConfigTopic(const QString &deviceId)
{
    return deviceTopic(deviceId, CONFIG);
}

QString PNDTopics::deviceAvailabilityTopic(const QString &deviceId)
{
    return deviceTopic(deviceId, AVAILABILITY);
}

QString PNDTopics::discoveryTopic()
{
    return QString("%1/%2").arg(ROOT, DISCOVERY);
}

bool PNDTopics::isDeviceTopic(const QString &topic, QString *deviceId)
{
    QRegularExpression re(QString("%1/device/([^/]+)(?:/.*)?").arg(ROOT));
    QRegularExpressionMatch match = re.match(topic);

    if (match.hasMatch()) {
        if (deviceId) {
            *deviceId = match.captured(1);
        }
        return true;
    }

    return false;
}

QString PNDTopics::extractDeviceId(const QString &topic)
{
    QString deviceId;
    isDeviceTopic(topic, &deviceId);
    return deviceId;
}
