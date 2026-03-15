#include "pnddevicemodel.h"
#include <QDebug>

PNDDeviceModel::PNDDeviceModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

PNDDeviceModel::~PNDDeviceModel()
{
    qDeleteAll(m_devices);
    m_devices.clear();
    m_deviceMap.clear();
}

int PNDDeviceModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_devices.size();
}

QVariant PNDDeviceModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_devices.size())
        return QVariant();

    PNDDevice *device = m_devices.at(index.row());

    switch (role) {
    case DeviceIdRole:
        return device->deviceId();
    case StateRole:
        return device->state();
    case TemperatureRole:
        return device->temperature();
    case HumidityRole:
        return device->humidity();
    case LastSeenRole:
        return device->lastSeen();
    case DeviceObjectRole:
        return QVariant::fromValue(device);
    case Qt::DisplayRole:
        return QString("%1 (T: %2°C, H: %3%%)")
            .arg(device->deviceId())
            .arg(device->temperature())
            .arg(device->humidity());
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> PNDDeviceModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[DeviceIdRole] = "deviceId";
    roles[StateRole] = "state";
    roles[TemperatureRole] = "temperature";
    roles[HumidityRole] = "humidity";
    roles[LastSeenRole] = "lastSeen";
    roles[DeviceObjectRole] = "deviceObject";
    return roles;
}

void PNDDeviceModel::addDevice(PNDDevice *device)
{
    if (!device || m_deviceMap.contains(device->deviceId())) {
        return;
    }

    beginInsertRows(QModelIndex(), m_devices.size(), m_devices.size());
    m_devices.append(device);
    m_deviceMap.insert(device->deviceId(), device);
    connectDeviceSignals(device);
    endInsertRows();

    emit countChanged();
    emit deviceAdded(device->deviceId());
}

void PNDDeviceModel::addDevice(const QString &deviceId)
{
    if (m_deviceMap.contains(deviceId)) {
        return;
    }

    PNDDevice *device = new PNDDevice(deviceId, this);
    addDevice(device);
}

void PNDDeviceModel::removeDevice(const QString &deviceId)
{
    if (!m_deviceMap.contains(deviceId)) {
        return;
    }

    PNDDevice *device = m_deviceMap.value(deviceId);
    int index = m_devices.indexOf(device);

    if (index >= 0) {
        beginRemoveRows(QModelIndex(), index, index);
        m_devices.removeAt(index);
        m_deviceMap.remove(deviceId);
        disconnectDeviceSignals(device);
        device->deleteLater();
        endRemoveRows();

        emit countChanged();
        emit deviceRemoved(deviceId);
    }
}

PNDDevice* PNDDeviceModel::getDevice(const QString &deviceId) const
{
    return m_deviceMap.value(deviceId, nullptr);
}

PNDDevice* PNDDeviceModel::getDevice(int index) const
{
    if (index < 0 || index >= m_devices.size()) {
        return nullptr;
    }
    return m_devices.at(index);
}

void PNDDeviceModel::clear()
{
    if (m_devices.isEmpty()) {
        return;
    }

    beginResetModel();

    for (PNDDevice *device : qAsConst(m_devices)) {
        disconnectDeviceSignals(device);
        device->deleteLater();
    }

    m_devices.clear();
    m_deviceMap.clear();

    endResetModel();

    emit countChanged();
}

bool PNDDeviceModel::contains(const QString &deviceId) const
{
    return m_deviceMap.contains(deviceId);
}

int PNDDeviceModel::indexOf(const QString &deviceId) const
{
    PNDDevice *device = m_deviceMap.value(deviceId, nullptr);
    if (device) {
        return m_devices.indexOf(device);
    }
    return -1;
}

void PNDDeviceModel::updateDeviceState(const QString &deviceId, int state)
{
    PNDDevice *device = getDevice(deviceId);
    if (device) {
        device->setState(static_cast<PNDDeviceState::State>(state));
    }
}

void PNDDeviceModel::updateDeviceSensors(const QString &deviceId, float temperature, float humidity)
{
    PNDDevice *device = getDevice(deviceId);
    if (device) {
        device->setTemperature(temperature);
        device->setHumidity(humidity);
    }
}

void PNDDeviceModel::updateDeviceFromJson(const QString &deviceId, const QByteArray &jsonData)
{
    PNDDevice *device = getDevice(deviceId);
    if (device) {
        device->updateFromJson(jsonData);
    }
}

void PNDDeviceModel::onDeviceDataChanged()
{
    PNDDevice *device = qobject_cast<PNDDevice*>(sender());
    if (device) {
        int index = m_devices.indexOf(device);
        if (index >= 0) {
            QModelIndex modelIndex = createIndex(index, 0);
            emit dataChanged(modelIndex, modelIndex);
            emit deviceUpdated(device->deviceId());
        }
    }
}

void PNDDeviceModel::connectDeviceSignals(PNDDevice *device)
{
    connect(device, &PNDDevice::stateChanged, this, &PNDDeviceModel::onDeviceDataChanged);
    connect(device, &PNDDevice::temperatureChanged, this, &PNDDeviceModel::onDeviceDataChanged);
    connect(device, &PNDDevice::humidityChanged, this, &PNDDeviceModel::onDeviceDataChanged);
    connect(device, &PNDDevice::lastSeenChanged, this, &PNDDeviceModel::onDeviceDataChanged);
    connect(device, &PNDDevice::dataUpdated, this, &PNDDeviceModel::onDeviceDataChanged);
}

void PNDDeviceModel::disconnectDeviceSignals(PNDDevice *device)
{
    disconnect(device, &PNDDevice::stateChanged, this, &PNDDeviceModel::onDeviceDataChanged);
    disconnect(device, &PNDDevice::temperatureChanged, this, &PNDDeviceModel::onDeviceDataChanged);
    disconnect(device, &PNDDevice::humidityChanged, this, &PNDDeviceModel::onDeviceDataChanged);
    disconnect(device, &PNDDevice::lastSeenChanged, this, &PNDDeviceModel::onDeviceDataChanged);
    disconnect(device, &PNDDevice::dataUpdated, this, &PNDDeviceModel::onDeviceDataChanged);
}
