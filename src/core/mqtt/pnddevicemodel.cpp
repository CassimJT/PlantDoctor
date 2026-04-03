#include "pnddevicemodel.h"
#include <QDebug>

PNDDeviceModel::PNDDeviceModel(QObject *parent)
    : QAbstractListModel(parent)
{
    qDebug() << "==================================================";
    qDebug() << "PNDDeviceModel initialized";
    qDebug() << "Initial row count:" << rowCount();
    qDebug() << "==================================================";
}

PNDDeviceModel::~PNDDeviceModel()
{
    qDeleteAll(m_devices);
}

int PNDDeviceModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_devices.size();
}

QVariant PNDDeviceModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_devices.size())
        return {};

    PNDDevice *device = m_devices.at(index.row());

    switch (role) {
    case DeviceIdRole: return device->deviceId();
    case StateRole: return device->state();
    case TemperatureRole: return device->temperature();
    case HumidityRole: return device->humidity();
    case LastSeenRole: return device->lastSeen();
    case DeviceObjectRole: return QVariant::fromValue(device);
    case Qt::DisplayRole:
        return QString("%1 (T:%2°C H:%3%)")
            .arg(device->deviceId())
            .arg(device->temperature())
            .arg(device->humidity());
    case Qt::UserRole:
        return device->deviceId();
    default:
        return {};
    }
}

QHash<int, QByteArray> PNDDeviceModel::roleNames() const
{
    return {
        {DeviceIdRole, "deviceId"},
        {StateRole, "state"},
        {TemperatureRole, "temperature"},
        {HumidityRole, "humidity"},
        {LastSeenRole, "lastSeen"},
        {DeviceObjectRole, "deviceObject"}
    };
}

void PNDDeviceModel::addDevice(PNDDevice* device)
{
    if (!device || m_deviceMap.contains(device->deviceId())) {
        qDebug() << "addDevice: Device" << (device ? device->deviceId() : "None") << "already exists or invalid";
        return;
    }

    qDebug() << "\n addDevice: Adding device" << device->deviceId();
    qDebug() << "addDevice: Current row count:" << rowCount();

    beginInsertRows(QModelIndex(), m_devices.size(), m_devices.size());
    m_devices.append(device);
    m_deviceMap.insert(device->deviceId(), device);
    connectDeviceSignals(device);
    endInsertRows();

    qDebug() << "addDevice: New row count:" << rowCount();
    qDebug() << "addDevice: Device map size:" << m_deviceMap.size();

    emit countChanged();
    emit deviceAdded(device->deviceId());

    debugPrintDevices();

    // Force a data change notification
    QModelIndex topLeft = index(0, 0);
    QModelIndex bottomRight = index(rowCount() - 1, 0);
    emit dataChanged(topLeft, bottomRight);
}

void PNDDeviceModel::addDeviceById(const QString &deviceId)
{
    qDebug() << "\n addDeviceById: Adding device" << deviceId;

    if (m_deviceMap.contains(deviceId)) {
        qDebug() << "addDeviceById: Device" << deviceId << "already exists";
        return;
    }

    PNDDevice* device = new PNDDevice(deviceId, this);
    addDevice(device);

    if (m_deviceMap.contains(deviceId)) {
        qDebug() << "addDeviceById: Device" << deviceId << "successfully added, new count:" << rowCount();
    } else {
        qDebug() << "addDeviceById: ERROR - Device" << deviceId << "not found after add!";
    }
}

void PNDDeviceModel::removeDevice(const QString &deviceId)
{
    qDebug() << "\nremoveDevice: Removing device" << deviceId;

    if (!m_deviceMap.contains(deviceId)) {
        qDebug() << "removeDevice: Device" << deviceId << "not found";
        return;
    }

    PNDDevice* device = m_deviceMap.value(deviceId);
    int index = m_devices.indexOf(device);

    qDebug() << "removeDevice: Found at index" << index;

    beginRemoveRows(QModelIndex(), index, index);
    m_devices.removeAt(index);
    m_deviceMap.remove(deviceId);
    disconnectDeviceSignals(device);
    device->deleteLater();
    endRemoveRows();

    qDebug() << "removeDevice: New row count:" << rowCount();

    debugPrintDevices();
    emit countChanged();
    emit deviceRemoved(deviceId);
}

PNDDevice* PNDDeviceModel::getDevice(const QString &deviceId) const
{
    return m_deviceMap.value(deviceId, nullptr);
}

PNDDevice* PNDDeviceModel::getDevice(int index) const
{
    if (index < 0 || index >= m_devices.size())
        return nullptr;
    return m_devices.at(index);
}

void PNDDeviceModel::clear()
{
    if (m_devices.isEmpty()) return;

    qDebug() << "\nclear: Removing all" << m_devices.size() << "devices";

    beginResetModel();

    for (PNDDevice* device : m_devices) {
        disconnectDeviceSignals(device);
        device->deleteLater();
    }

    m_devices.clear();
    m_deviceMap.clear();

    endResetModel();

    qDebug() << "clear: Model cleared, count:" << rowCount();
    debugPrintDevices();
    emit countChanged();
}

bool PNDDeviceModel::contains(const QString &deviceId) const
{
    return m_deviceMap.contains(deviceId);
}

int PNDDeviceModel::indexOf(const QString &deviceId) const
{
    PNDDevice* device = m_deviceMap.value(deviceId, nullptr);
    if (device)
        return m_devices.indexOf(device);
    return -1;
}

void PNDDeviceModel::updateDeviceState(const QString &deviceId, int state)
{
    PNDDevice* device = getDevice(deviceId);
    if (device)
        device->setState(static_cast<PNDDeviceState::State>(state));
}

void PNDDeviceModel::updateDeviceSensors(const QString &deviceId, float temperature, float humidity)
{
    PNDDevice* device = getDevice(deviceId);
    if (device) {
        device->setTemperature(temperature);
        device->setHumidity(humidity);
    }
}

void PNDDeviceModel::updateDeviceFromJson(const QString &deviceId, const QByteArray &jsonData)
{
    PNDDevice* device = getDevice(deviceId);
    if (device)
        device->updateFromJson(jsonData);
}

void PNDDeviceModel::debugPrintDevices()
{
    qDebug() << "\n" << QString(60, '=');
    qDebug() << "DEBUG: Model has" << rowCount() << "devices:";
    if (rowCount() == 0) {
        qDebug() << "  (no devices)";
    } else {
        for (int i = 0; i < m_devices.size(); ++i) {
            PNDDevice* device = m_devices[i];
            QString stateStr;
            switch (device->state()) {
            case PNDDeviceState::DISCONNECTED: stateStr = "DISCONNECTED"; break;
            case PNDDeviceState::CONNECTING: stateStr = "CONNECTING"; break;
            case PNDDeviceState::CONNECTED: stateStr = "CONNECTED"; break;
            case PNDDeviceState::ERROR: stateStr = "ERROR"; break;
            default: stateStr = "UNKNOWN";
            }
            qDebug() << QString("  [%1] %2").arg(i).arg(device->deviceId());
            qDebug() << QString("       State: %1 (%2)").arg(stateStr).arg(device->state());
            qDebug() << QString("       Temp: %1°C, Hum: %2%").arg(device->temperature()).arg(device->humidity());
            qDebug() << QString("       LastSeen: %1").arg(device->lastSeen().toString());
        }
    }
    qDebug() << QString(60, '=') << "\n";
}

void PNDDeviceModel::onDeviceDataChanged()
{
    PNDDevice* device = qobject_cast<PNDDevice*>(sender());
    if (device && m_deviceMap.contains(device->deviceId())) {
        int index = m_devices.indexOf(device);
        if (index >= 0) {
            QModelIndex modelIndex = createIndex(index, 0);
            emit dataChanged(modelIndex, modelIndex);
            emit deviceUpdated(device->deviceId());
            qDebug() << "Device" << device->deviceId() << "data changed, view updated at index" << index;

            QString stateStr;
            switch (device->state()) {
            case PNDDeviceState::DISCONNECTED: stateStr = "DISCONNECTED"; break;
            case PNDDeviceState::CONNECTING: stateStr = "CONNECTING"; break;
            case PNDDeviceState::CONNECTED: stateStr = "CONNECTED"; break;
            case PNDDeviceState::ERROR: stateStr = "ERROR"; break;
            default: stateStr = "UNKNOWN";
            }
            qDebug() << QString("   New state: %1, Temp: %2°C, Hum: %3%")
                            .arg(stateStr)
                            .arg(device->temperature())
                            .arg(device->humidity());
        }
    }
}

void PNDDeviceModel::connectDeviceSignals(PNDDevice *device)
{
    connect(device, &PNDDevice::dataUpdated, this, &PNDDeviceModel::onDeviceDataChanged);
}

void PNDDeviceModel::disconnectDeviceSignals(PNDDevice *device)
{
    disconnect(device, &PNDDevice::dataUpdated, this, &PNDDeviceModel::onDeviceDataChanged);
}