#include "StatusControl.h"

const char* StatusControl::stateToString(DeviceState state) {
    switch (state) {
        case POWER_ON: return "POWER_ON";
        case WIFI_CONNECTING: return "WIFI_CONNECTING";
        case STREAM_BUFFERING: return "STREAM_BUFFERING";
        case STREAM_CONNECTION_FAILED: return "STREAM_CONNECTION_FAILED";
        case PLAYING: return "PLAYING";
        case MUTED: return "MUTED";
        case STOPPED: return "STOPPED";
        case VOLUME_CHANGE: return "VOLUME_CHANGE";
        case STATION_CHANGE: return "STATION_CHANGE";
        case FACTORY_RESET_COUNTDOWN: return "FACTORY_RESET_COUNTDOWN";
        default: return "UNKNOWN";
    }
}

StatusControl::StatusControl() {
    setState(POWER_ON);
}

void StatusControl::setState(DeviceState newState) {
    if (currentState != newState) {
        currentState = newState;
        Serial.println("StatusControl: state changed to " + String(stateToString(currentState)));
    }
}

DeviceState StatusControl::getState() {
    return currentState;
} 