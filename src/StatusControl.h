#ifndef STATUSCONTROL_H
#define STATUSCONTROL_H

#include <Arduino.h>

enum DeviceState {
    POWER_ON,
    WIFI_CONNECTING,
    STREAM_BUFFERING,
    STREAM_CONNECTION_FAILED,
    PLAYING,
    MUTED,
    VOLUME_CHANGE,
    FACTORY_RESET_COUNTDOWN
};

class StatusControl {
public:
    StatusControl();
    void setState(DeviceState newState);
    DeviceState getState();

private:
    DeviceState currentState;
    const char* stateToString(DeviceState state);
};

#endif // STATUSCONTROL_H 