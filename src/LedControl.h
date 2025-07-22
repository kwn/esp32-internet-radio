#ifndef LEDCONTROL_H
#define LEDCONTROL_H

#include <FastLED.h>
#include <Arduino.h>
#include "StatusControl.h"
#include "StationControl.h"
#include "Debouncer.h"

enum OverlayType {
    OVERLAY_NONE,
    OVERLAY_VOLUME
};

class LedControl {
public:
    LedControl(CRGB* leds, int numLeds, StatusControl* statusControl, StationControl* stationControl);
    void update();
    void triggerVolumeOverlay(int volume);

private:
    CRGB* leds;
    int numLeds;
    StatusControl* statusControl;
    StationControl* stationControl;
    int animationStep;
    int direction;
    Debouncer animationDebouncer;
    unsigned long overlayTimeout;
    OverlayType activeOverlay;
    int overlayValue;

    // Effect methods
    void displayPrimaryState();
    void displayOverlay();
    void showPowerOn();
    void showWifiConnecting();
    void showStreamBuffering();
    void showPlaying();
    void showMuted();
    void showFactoryReset();
    void showError();

    void clear();
};

#endif // LEDCONTROL_H
