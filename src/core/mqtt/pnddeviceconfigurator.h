#ifndef PNDDEVICECONFIGURATOR_H
#define PNDDEVICECONFIGURATOR_H

#include <QObject>

class PNDDeviceConfigurator : public QObject
{
    Q_OBJECT
public:
    explicit PNDDeviceConfigurator(QObject *parent = nullptr);

signals:
};

#endif // PNDDEVICECONFIGURATOR_H
