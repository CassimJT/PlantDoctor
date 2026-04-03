#ifndef PNDDEVICESTATE_H
#define PNDDEVICESTATE_H

#include <QObject>

// Class to hold device states for Qt meta-object system
class PNDDeviceState
{
    Q_GADGET

public:
    enum State {
        DISCONNECTED = 0,
        CONNECTING   = 1,
        CONNECTED    = 2,
        ERROR        = 3
    };
    Q_ENUM(State)  // Registers enum with Qt meta system
};

#endif // PNDDEVICESTATE_H