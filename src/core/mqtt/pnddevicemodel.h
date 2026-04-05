#ifndef PNDDEVICEMODEL_H
#define PNDDEVICEMODEL_H

#include <QAbstractListModel>
#include <QHash>
#include <QList>
#include "pnddevice.h"
#include "pnddevicestate.h"

class PNDDeviceModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)

public:
    enum DeviceRoles {
        DeviceIdRole = Qt::UserRole + 1,
        StateRole = Qt::UserRole + 2,
        TemperatureRole = Qt::UserRole + 3,
        HumidityRole = Qt::UserRole + 4,
        LastSeenRole = Qt::UserRole + 5,
        DeviceObjectRole = Qt::UserRole + 6
    };

    explicit PNDDeviceModel(QObject *parent = nullptr);
    ~PNDDeviceModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    // Q_INVOKABLE methods for QML
    Q_INVOKABLE void addDevice(PNDDevice* device);
    Q_INVOKABLE void addDeviceById(const QString &deviceId);
    Q_INVOKABLE void removeDevice(const QString &deviceId);
    Q_INVOKABLE PNDDevice* getDevice(const QString &deviceId) const;
    Q_INVOKABLE PNDDevice* getDevice(int index) const;
    Q_INVOKABLE void clear();
    Q_INVOKABLE bool contains(const QString &deviceId) const;
    Q_INVOKABLE int indexOf(const QString &deviceId) const;

    Q_INVOKABLE void updateDeviceState(const QString &deviceId, int state);
    Q_INVOKABLE void updateDeviceSensors(const QString &deviceId, float temperature, float humidity);
    Q_INVOKABLE void updateDeviceFromJson(const QString &deviceId, const QByteArray &jsonData);

    Q_INVOKABLE void debugPrintDevices();

signals:
    void countChanged();
    void deviceAdded(const QString &deviceId);
    void deviceRemoved(const QString &deviceId);
    void deviceUpdated(const QString &deviceId);

private slots:
    void onDeviceDataChanged();

private:
    QList<PNDDevice*> m_devices;
    QHash<QString, PNDDevice*> m_deviceMap;

    void connectDeviceSignals(PNDDevice *device);
    void disconnectDeviceSignals(PNDDevice *device);
};

#endif