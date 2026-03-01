#ifndef PNDDEVICEMODEL_H
#define PNDDEVICEMODEL_H

#include <QAbstractListModel>

class PNDDeviceModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit PNDDeviceModel(QObject *parent = nullptr);

    // Header:
    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
};

#endif // PNDDEVICEMODEL_H
