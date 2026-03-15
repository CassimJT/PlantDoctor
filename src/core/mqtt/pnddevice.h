#ifndef PNDDEVICE_H
#define PNDDEVICE_H

#include <QObject>
#include "pnddevicestate.h"

class PNDDevice : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString deviceId READ deviceId WRITE setDeviceId NOTIFY deviceIdChanged)
    Q_PROPERTY(int state READ state NOTIFY stateChanged)  // Keep as int for QML
    Q_PROPERTY(float temperature READ temperature WRITE setTemperature NOTIFY temperatureChanged)
    Q_PROPERTY(float humidity READ humidity WRITE setHumidity NOTIFY humidityChanged)
    Q_PROPERTY(QString lastSeen READ lastSeen WRITE setLastSeen NOTIFY lastSeenChanged)

public:
    explicit PNDDevice(const QString &deviceId = QString(), QObject *parent = nullptr);

    QString deviceId() const;
    void setDeviceId(const QString &deviceId);

    int state() const;  // Returns int for QML compatibility
    void setState(PNDDeviceState::State state);  // Takes enum internally

    float temperature() const;
    void setTemperature(float temperature);

    float humidity() const;
    void setHumidity(float humidity);

    QString lastSeen() const;
    void setLastSeen(const QString &lastSeen);

    Q_INVOKABLE void updateFromJson(const QByteArray &jsonData);
    Q_INVOKABLE QByteArray toJson() const;

signals:
    void deviceIdChanged();
    void stateChanged();
    void temperatureChanged();
    void humidityChanged();
    void lastSeenChanged();
    void dataUpdated();

private:
    QString m_deviceId;
    PNDDeviceState::State m_state;
    float m_temperature;
    float m_humidity;
    QString m_lastSeen;
};

// No Q_DECLARE_METATYPE here - it's already in pnddevicestate.h

#endif // PNDDEVICE_H
