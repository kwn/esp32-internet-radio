#ifndef LEDCONTROL_H
#define LEDCONTROL_H

#include <FastLED.h>
#include <Arduino.h>
#include "StatusControl.h"
#include "StationControl.h"
#include "Debouncer.h"

enum OverlayType {
    OVERLAY_NONE,
    OVERLAY_VOLUME,
    OVERLAY_TONE
};

class LedControl {
public:
    LedControl(CRGB* leds, int numLeds, StatusControl* statusControl, StationControl* stationControl);
    void update();
    void triggerVolumeOverlay(int volume);
    void triggerToneOverlay(int tone);

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
    void showWifiConnecting();
    void showStreamBuffering();
    void showPlaying();
    void showMuted();
    void showFactoryReset();

    void clear();
};

#endif // LEDCONTROL_H
