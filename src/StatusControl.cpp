#include "StatusControl.h"

const char* StatusControl::stateToString(PrimaryState state) {
    switch (state) {
        case STATE_POWERING_ON: return "POWERING_ON";
        case STATE_WIFI_CONNECTING: return "WIFI_CONNECTING";
        case STATE_STREAM_BUFFERING: return "STREAM_BUFFERING";
        case STATE_PLAYING: return "PLAYING";
        default: return "UNKNOWN";
    }
}

StatusControl::StatusControl():
    currentState(STATE_POWERING_ON), muted(false), factoryResetting(false) {
}

// --- Primary State ---
void StatusControl::setPrimaryState(PrimaryState newState) {
    if (currentState != newState) {
        currentState = newState;
        Serial.println("StatusControl: state changed to " + String(stateToString(currentState)));
    }
}

PrimaryState StatusControl::getPrimaryState() {
    return currentState;
}

// --- Status Flags ---
void StatusControl::setMuted(bool isMuted) {
    if (muted != isMuted) {
        muted = isMuted;
        Serial.println(muted ? "StatusControl: Muted" : "StatusControl: Unmuted");
    }
}

bool StatusControl::isMuted() {
    return muted;
}

void StatusControl::setFactoryReset(bool isResetting) {
    if (factoryResetting != isResetting) {
        factoryResetting = isResetting;
        if (factoryResetting) {
            Serial.println("StatusControl: Factory reset countdown started");
        } else {
            Serial.println("StatusControl: Factory reset cancelled");
        }
    }
}

bool StatusControl::isFactoryResetting() {
    return factoryResetting;
}