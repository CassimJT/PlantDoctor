#ifndef PNDDEVICE_H
#define PNDDEVICE_H

#include <QObject>
#include <QDateTime>
#include "pnddevicestate.h"

class PNDDevice : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString deviceId READ deviceId WRITE setDeviceId NOTIFY deviceIdChanged)
    Q_PROPERTY(int state READ state NOTIFY stateChanged)
    Q_PROPERTY(float temperature READ temperature NOTIFY temperatureChanged)
    Q_PROPERTY(float humidity READ humidity NOTIFY humidityChanged)
    Q_PROPERTY(QDateTime lastSeen READ lastSeen NOTIFY lastSeenChanged)

public:
    explicit PNDDevice(const QString &deviceId = QString(), QObject *parent = nullptr);

    QString deviceId() const;
    void setDeviceId(const QString &deviceId);

    int state() const;
    void setState(PNDDeviceState::State state);

    float temperature() const;
    void setTemperature(float temperature);

    float humidity() const;
    void setHumidity(float humidity);

    QDateTime lastSeen() const;
    void setLastSeen(const QDateTime &lastSeen);

    Q_INVOKABLE void updateFromJson(const QByteArray &jsonData);
    Q_INVOKABLE QByteArray toJson() const;

signals:
    void deviceIdChanged();
    void stateChanged();
    void temperatureChanged();
    void humidityChanged();
    void lastSeenChanged();

    // 🔥 IMPORTANT: single aggregation signal (used by model)
    void dataUpdated();

private:
    QString m_deviceId;
    PNDDeviceState::State m_state;

    float m_temperature;
    float m_humidity;

    QDateTime m_lastSeen;
};

#endif // PNDDEVICE_H