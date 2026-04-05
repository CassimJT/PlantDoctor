#include "pnddevice.h"
#include <QJsonDocument>
#include <QJsonObject>

PNDDevice::PNDDevice(const QString &deviceId, QObject *parent)
    : QObject(parent),
    m_deviceId(deviceId),
    m_state(PNDDeviceState::DISCONNECTED),
    m_temperature(0.0f),
    m_humidity(0.0f),
    m_lastSeen(QDateTime::currentDateTime())
{
}


// ==============================
// Getters / Setters
// ==============================

QString PNDDevice::deviceId() const
{
    return m_deviceId;
}

void PNDDevice::setDeviceId(const QString &deviceId)
{
    if (m_deviceId == deviceId)
        return;

    m_deviceId = deviceId;
    emit deviceIdChanged();
}


int PNDDevice::state() const
{
    return static_cast<int>(m_state);
}

void PNDDevice::setState(PNDDeviceState::State state)
{
    if (m_state == state)
        return;

    m_state = state;
    emit stateChanged();
    emit dataUpdated();
}


float PNDDevice::temperature() const
{
    return m_temperature;
}

void PNDDevice::setTemperature(float temperature)
{
    if (qFuzzyCompare(m_temperature, temperature))
        return;

    m_temperature = temperature;
    emit temperatureChanged();
    emit dataUpdated();
}


float PNDDevice::humidity() const
{
    return m_humidity;
}

void PNDDevice::setHumidity(float humidity)
{
    if (qFuzzyCompare(m_humidity, humidity))
        return;

    m_humidity = humidity;
    emit humidityChanged();
    emit dataUpdated();
}


QDateTime PNDDevice::lastSeen() const
{
    return m_lastSeen;
}

void PNDDevice::setLastSeen(const QDateTime &lastSeen)
{
    if (m_lastSeen == lastSeen)
        return;

    m_lastSeen = lastSeen;
    emit lastSeenChanged();
    emit dataUpdated();   // 🔥 REQUIRED for model sync
}


// ==============================
// JSON Handling
// ==============================

void PNDDevice::updateFromJson(const QByteArray &jsonData)
{
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(jsonData, &err);

    if (err.error != QJsonParseError::NoError || !doc.isObject()) {
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
        int s = obj["state"].toInt();
        if (s >= PNDDeviceState::DISCONNECTED && s <= PNDDeviceState::ERROR) {
            setState(static_cast<PNDDeviceState::State>(s));
        }
    }

    // Always refresh lastSeen on valid update
    setLastSeen(QDateTime::currentDateTime());
}


QByteArray PNDDevice::toJson() const
{
    QJsonObject obj;

    obj["deviceId"] = m_deviceId;
    obj["state"] = static_cast<int>(m_state);
    obj["temperature"] = m_temperature;
    obj["humidity"] = m_humidity;
    obj["lastSeen"] = m_lastSeen.toString(Qt::ISODate);

    return QJsonDocument(obj).toJson(QJsonDocument::Compact);
}