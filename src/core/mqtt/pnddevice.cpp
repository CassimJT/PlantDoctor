#include "pnddevice.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QDateTime>

PNDDevice::PNDDevice(const QString &deviceId, QObject *parent)
    : QObject(parent)
    , m_deviceId(deviceId)
    , m_state(PNDDeviceState::DISCONNECTED)
    , m_temperature(0.0f)
    , m_humidity(0.0f)
    , m_lastSeen(QDateTime::currentDateTime().toString(Qt::ISODate))
{
}

QString PNDDevice::deviceId() const
{
    return m_deviceId;
}

void PNDDevice::setDeviceId(const QString &deviceId)
{
    if (m_deviceId != deviceId) {
        m_deviceId = deviceId;
        emit deviceIdChanged();
    }
}

int PNDDevice::state() const
{
    return static_cast<int>(m_state);  // Convert to int for QML
}

void PNDDevice::setState(PNDDeviceState::State state)
{
    if (m_state != state) {
        m_state = state;
        emit stateChanged();
        emit dataUpdated();
    }
}

float PNDDevice::temperature() const
{
    return m_temperature;
}

void PNDDevice::setTemperature(float temperature)
{
    if (!qFuzzyCompare(m_temperature, temperature)) {
        m_temperature = temperature;
        emit temperatureChanged();
        emit dataUpdated();
    }
}

float PNDDevice::humidity() const
{
    return m_humidity;
}

void PNDDevice::setHumidity(float humidity)
{
    if (!qFuzzyCompare(m_humidity, humidity)) {
        m_humidity = humidity;
        emit humidityChanged();
        emit dataUpdated();
    }
}

QString PNDDevice::lastSeen() const
{
    return m_lastSeen;
}

void PNDDevice::setLastSeen(const QString &lastSeen)
{
    if (m_lastSeen != lastSeen) {
        m_lastSeen = lastSeen;
        emit lastSeenChanged();
    }
}

void PNDDevice::updateFromJson(const QByteArray &jsonData)
{
    QJsonDocument doc = QJsonDocument::fromJson(jsonData);
    if (!doc.isObject()) {
        return;
    }

    QJsonObject obj = doc.object();

    if (obj.contains("temperature") && obj["temperature"].isDouble()) {
        setTemperature(static_cast<float>(obj["temperature"].toDouble()));
    }

    if (obj.contains("humidity") && obj["humidity"].isDouble()) {
        setHumidity(static_cast<float>(obj["humidity"].toDouble()));
    }

    if (obj.contains("state") && obj["state"].isDouble()) {
        int state = obj["state"].toInt();
        if (state >= 0 && state <= 3) {
            setState(static_cast<PNDDeviceState::State>(state));
        }
    }

    setLastSeen(QDateTime::currentDateTime().toString(Qt::ISODate));
}

QByteArray PNDDevice::toJson() const
{
    QJsonObject obj;
    obj["deviceId"] = m_deviceId;
    obj["state"] = static_cast<int>(m_state);
    obj["temperature"] = static_cast<double>(m_temperature);
    obj["humidity"] = static_cast<double>(m_humidity);
    obj["lastSeen"] = m_lastSeen;

    return QJsonDocument(obj).toJson();
}
