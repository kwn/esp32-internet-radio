#include "StatusControl.h"

StatusControl::StatusControl() {
    // Set initial state
    currentState = POWER_ON;
}

void StatusControl::setState(DeviceState newState) {
    if (currentState != newState) {
        currentState = newState;
    }
}

DeviceState StatusControl::getState() {
    return currentState;
} 