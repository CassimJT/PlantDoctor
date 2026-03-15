#ifndef PNDDEVICESTATE_H
#define PNDDEVICESTATE_H

#include <QObject>

// Use Q_NAMESPACE for proper enum handling in Qt6
class PNDDeviceState
{
    Q_GADGET

public:
    enum State {
        DISCONNECTED = 0,
        CONNECTING = 1,
        CONNECTED = 2,
        ERROR = 3
    };
    Q_ENUM(State)  // This registers the enum with Qt's meta system

private:
    explicit PNDDeviceState() = default;
};

#endif // PNDDEVICESTATE_H
