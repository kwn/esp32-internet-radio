#ifndef LEDCONTROL_H
#define LEDCONTROL_H

#include <FastLED.h>
#include <Arduino.h>
#include "StatusControl.h"
#include "StationControl.h"
#include "Debouncer.h"

class LedControl {
public:
    LedControl(CRGB* leds, int numLeds, StatusControl* statusControl, StationControl* stationControl);
    void update();

private:
    CRGB* leds;
    int numLeds;
    StatusControl* statusControl;
    StationControl* stationControl;
    int animationStep;
    int direction;
    Debouncer animationDebouncer;

    // Effect methods
    void showPowerOn();
    void showWifiConnecting();
    void showStreamBuffering();
    void showPlaying();
    void showMuted();
    void showStopped();
    void showVolumeChange();
    void showStationChange();
    void showFactoryResetCountdown();

    void clear();
};

#endif // LEDCONTROL_H
