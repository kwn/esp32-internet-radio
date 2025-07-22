#include "LedControl.h"

LedControl::LedControl(CRGB* leds, int numLeds, StatusControl* statusControl, StationControl* stationControl)
    : leds(leds), numLeds(numLeds), statusControl(statusControl), stationControl(stationControl), animationStep(0), direction(1), animationDebouncer(60), overlayTimeout(0) {}

void LedControl::update() {
    if (millis() < overlayTimeout) {
        displayOverlay();
    } else if (statusControl->isMuted()) {
        showMuted();
    } else if (statusControl->isFactoryResetting()) {
        showFactoryReset();
    }
    else {
        displayPrimaryState();
    }
    FastLED.show();
}

void LedControl::triggerOverlay() {
    overlayTimeout = millis() + 2000; // Overlay for 2 seconds
}

void LedControl::displayPrimaryState() {
    PrimaryState currentState = statusControl->getPrimaryState();

    switch (currentState) {
        case STATE_POWERING_ON:
            showPowerOn();
            break;
        case STATE_WIFI_CONNECTING:
            showWifiConnecting();
            break;
        case STATE_STREAM_BUFFERING:
            showStreamBuffering();
            break;
        case STATE_PLAYING:
            showPlaying();
            break;
        case STATE_ERROR:
            showError();
            break;
    }
}

void LedControl::displayOverlay() {
    // For now, simple placeholder. We can restore detailed overlays later.
    clear();
    leds[0] = CRGB::Green;
    leds[1] = CRGB::Green;
    leds[2] = CRGB::Green;
}

void LedControl::clear() {
    fill_solid(leds, numLeds, CRGB::Black);
}

void LedControl::showPowerOn() {
    // Quick wipe animation...
    const int tailLength = 3;
    for (int i = numLeds + tailLength; i >= 0; i--) {
        clear();
        if (i < numLeds) { leds[i] = CRGB(20, 8, 0); }
        for (int j = 1; j <= tailLength; j++) {
            int tailIndex = i + j;
            if (tailIndex < numLeds) {
                leds[tailIndex] = CRGB(20, 8, 0);
                leds[tailIndex].nscale8(255 - (j * (255 / (tailLength + 1))));
            }
        }
        FastLED.show();
        delay(30);
    }
    clear();
}

void LedControl::showWifiConnecting() {
    if (animationDebouncer.hasElapsed()) {
        clear();
        leds[animationStep] = CHSV(160, 255, 255);
        for (int i = 1; i <= 3; i++) {
            int tailIndex = animationStep - (i * direction);
            if (tailIndex >= 0 && tailIndex < numLeds) {
                leds[tailIndex] = CHSV(160, 255, 255 - (i * 60));
            }
        }
        if (animationStep >= numLeds - 1) { direction = -1; }
        else if (animationStep <= 0) { direction = 1; }
        animationStep += direction;
    }
}

void LedControl::showStreamBuffering() {
    fill_solid(leds, numLeds, CRGB(20, 8, 0));
    int station = stationControl->getStationNumber();
    int ledIndex = numLeds - 1 - station;
    uint8_t brightness = beatsin8(60, 0, 255);
    if (ledIndex >= 0 && ledIndex < numLeds) {
        leds[ledIndex] = CHSV(30, 255, brightness);
    }
}

void LedControl::showPlaying() {
    fill_solid(leds, numLeds, CRGB(20, 8, 0));
    int station = stationControl->getStationNumber();
    int ledIndex = numLeds - 1 - station;
    if (ledIndex >= 0 && ledIndex < numLeds) {
        leds[ledIndex] = CRGB::Orange;
    }
}

void LedControl::showMuted() {
    fill_solid(leds, numLeds, CRGB::Red);
}

void LedControl::showFactoryReset() {
    // Simple pulsing red for factory reset
    uint8_t brightness = beatsin8(120, 50, 255); // Pulse twice per second
    fill_solid(leds, numLeds, CRGB(brightness, 0, 0));
}

void LedControl::showError() {
    // Blinking red for error
    if (animationDebouncer.hasElapsed()) {
        static bool on = false;
        on = !on;
        fill_solid(leds, numLeds, on ? CRGB::Red : CRGB::Black);
    }
}

