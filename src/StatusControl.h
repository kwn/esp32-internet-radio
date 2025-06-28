#ifndef STATUSCONTROL_H
#define STATUSCONTROL_H

enum DeviceState {
    POWER_ON,
    WIFI_CONNECTING,
    STREAM_BUFFERING,
    STREAM_CONNECTION_FAILED,
    PLAYING,
    MUTED,
    STOPPED,
    VOLUME_CHANGE,
    STATION_CHANGE,
    FACTORY_RESET_COUNTDOWN
};

class StatusControl {
public:
    StatusControl();
    void setState(DeviceState newState);
    DeviceState getState();

private:
    DeviceState currentState;
};

#endif // STATUSCONTROL_H 