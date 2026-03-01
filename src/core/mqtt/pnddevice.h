#ifndef PNDDEVICE_H
#define PNDDEVICE_H

#include <QObject>

class PNDDevice : public QObject
{
    Q_OBJECT
public:
    explicit PNDDevice(QObject *parent = nullptr);

signals:
};

#endif // PNDDEVICE_H
