#include "LedControl.h"

LedControl::LedControl(CRGB* leds, int numLeds, StatusControl* statusControl, StationControl* stationControl)
    : leds(leds), numLeds(numLeds), statusControl(statusControl), stationControl(stationControl), animationStep(0), direction(1), animationDebouncer(60), overlayTimeout(0), activeOverlay(OVERLAY_NONE), overlayValue(0) {}

void LedControl::update() {
    if (statusControl->isMuted()) { // Highest priority: Mute status
        showMuted();
    } else if (millis() < overlayTimeout) { // Second priority: Overlays
        displayOverlay();
    } else if (statusControl->isFactoryResetting()) { // Third priority: Factory reset
        showFactoryReset();
    } else { // Default: Primary state display
        if (activeOverlay != OVERLAY_NONE) {
            activeOverlay = OVERLAY_NONE;
        }

        displayPrimaryState();
    }

    FastLED.show();
}

void LedControl::triggerVolumeOverlay(int volume) {
    activeOverlay = OVERLAY_VOLUME;
    overlayValue = volume;
    overlayTimeout = millis() + 2000;
}

void LedControl::triggerToneOverlay(int tone) {
    activeOverlay = OVERLAY_TONE;
    overlayValue = tone;
    overlayTimeout = millis() + 2000;
}

void LedControl::displayPrimaryState() {
    PrimaryState currentState = statusControl->getPrimaryState();

    switch (currentState) {
        case STATE_WIFI_CONNECTING:
            showWifiConnecting();
            break;
        case STATE_STREAM_BUFFERING:
            showStreamBuffering();
            break;
        case STATE_PLAYING:
            showPlaying();
            break;
    }
}

void LedControl::displayOverlay() {
    clear();

    switch (activeOverlay) {
        case OVERLAY_VOLUME: {
            int ledsToShow = overlayValue;
            for (int i = 0; i < ledsToShow; i++) {
                leds[numLeds - 1 - i] = CRGB::Cyan;
            }
            break;
        }
        case OVERLAY_TONE: {
            int centerLed = numLeds / 2;
            int toneValue = overlayValue;

            leds[centerLed] = CRGB::White;

            if (toneValue > 0) { // Treble to the left (Orange) - physically right
                int ledsToLight = map(toneValue, 1, 5, 1, centerLed);
                for(int i = 1; i <= ledsToLight; i++) {
                    leds[centerLed - i].setHue(HUE_ORANGE);
                    leds[centerLed - i].nscale8(255 - (i * (255 / (ledsToLight + 1))));
                }
            } else if (toneValue < 0) { // Bass to the right (Blue) - physically left
                int ledsToLight = map(abs(toneValue), 1, 5, 1, numLeds - 1 - centerLed);
                for(int i = 1; i <= ledsToLight; i++) {
                    leds[centerLed + i].setHue(HUE_BLUE);
                    leds[centerLed + i].nscale8(255 - (i * (255 / (ledsToLight + 1))));
                }
            }
            break;
        }
        case OVERLAY_NONE:
        default:
            break;
    }
}

void LedControl::clear() {
    fill_solid(leds, numLeds, CRGB::Black);
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
    uint8_t brightness = beatsin8(120, 50, 255);
    fill_solid(leds, numLeds, CRGB(brightness, 0, 0));
}

