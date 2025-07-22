#ifndef STATUSCONTROL_H
#define STATUSCONTROL_H

#include <Arduino.h>

// Enum for primary, mutually exclusive states
enum PrimaryState {
    STATE_POWERING_ON,
    STATE_WIFI_CONNECTING,
    STATE_STREAM_BUFFERING,
    STATE_PLAYING,
    STATE_ERROR
};

class StatusControl {
public:
    StatusControl();

    // --- Primary State ---
    void setPrimaryState(PrimaryState newState);
    PrimaryState getPrimaryState();

    // --- Status Flags ---
    void setMuted(bool muted);
    bool isMuted();

    void setFactoryReset(bool resetting);
    bool isFactoryResetting();

private:
    PrimaryState currentState;
    bool muted;
    bool factoryResetting;

    const char* stateToString(PrimaryState state);
};

#endif // STATUSCONTROL_H 