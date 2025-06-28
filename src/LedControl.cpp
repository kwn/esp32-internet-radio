#include "LedControl.h"

LedControl::LedControl(CRGB* leds, int numLeds, StatusControl* statusControl)
    : leds(leds), numLeds(numLeds), statusControl(statusControl), lastUpdate(0), animationStep(0), direction(1) {}

void LedControl::update() {
    DeviceState currentState = statusControl->getState();

    switch (currentState) {
        case POWER_ON:
            showPowerOn();
            break;
        case WIFI_CONNECTING:
            showWifiConnecting();
            break;
        case STREAM_BUFFERING:
            showStreamBuffering();
            break;
        case PLAYING:
            showPlaying();
            break;
        case MUTED:
            showMuted();
            break;
        case STOPPED:
            showStopped();
            break;
        case VOLUME_CHANGE:
            // These are transient and handled differently, maybe via direct calls.
            // For now, they can fall through or show a default state.
            break;
        case STATION_CHANGE:
            // Handled as a transient effect.
            break;
        case FACTORY_RESET_COUNTDOWN:
            // This would be triggered by StationControl, needs specific handling.
            break;
        default:
            clear();
            break;
    }
    FastLED.show();
}

void LedControl::clear() {
    fill_solid(leds, numLeds, CRGB::Black);
}

void LedControl::showPowerOn() {
    fill_solid(leds, numLeds, CRGB::White);
    FastLED.show();
    delay(100); // Brief flash
    clear();
    FastLED.show();
}

void LedControl::showWifiConnecting() {
    unsigned long currentTime = millis();
    if (currentTime - lastUpdate > 60) { // Controls the speed of the scanner
        lastUpdate = currentTime;

        // Clear all LEDs to ensure a clean trail
        clear();

        // Draw the scanner with a gradient tail.
        // The 'direction' variable makes the tail follow the head correctly.
        
        // Head of the scanner (brightest)
        leds[animationStep] = CRGB::Blue;

        // Trail segment 1
        int tail1 = animationStep - (1 * direction);
        if (tail1 >= 0 && tail1 < numLeds) {
            leds[tail1] = CRGB(0, 0, 100); // Medium Blue
        }

        // Trail segment 2
        int tail2 = animationStep - (2 * direction);
        if (tail2 >= 0 && tail2 < numLeds) {
            leds[tail2] = CRGB(0, 0, 50); // Dim Blue
        }

        // Trail segment 3
        int tail3 = animationStep - (3 * direction);
        if (tail3 >= 0 && tail3 < numLeds) {
            leds[tail3] = CRGB(0, 0, 20); // Dimmest Blue
        }

        // Move the scanner head
        animationStep += direction;

        // Reverse direction if it hits either end of the strip
        if (animationStep <= 0 || animationStep >= numLeds - 1) {
            direction *= -1;
        }
    }
}

void LedControl::showStreamBuffering() {
    unsigned long currentTime = millis();
    if (currentTime - lastUpdate > 50) { // Chase animation
        lastUpdate = currentTime;
        for (int i = 0; i < numLeds; i++) {
            leds[i].fadeToBlackBy(40);
        }
        leds[animationStep] = CRGB::YellowGreen;
        animationStep = (animationStep + 1) % numLeds;
    }
}

void LedControl::showPlaying() {
    fill_solid(leds, numLeds, CRGB::Green);
    FastLED.setBrightness(40);
}

void LedControl::showMuted() {
    fill_solid(leds, numLeds, CRGB::Red);
    FastLED.setBrightness(40);
}

void LedControl::showStopped() {
    fill_solid(leds, numLeds, CRGB::DimGray);
}

// Note: Volume, Station, and Factory Reset require more complex integration
// as they are triggered by user actions, not just a continuous state.
// These are placeholders for now.
void LedControl::showVolumeChange() {
    // Example: To be called directly with a value
}

void LedControl::showStationChange() {
    // Example: To be called directly with a value
}

void LedControl::showFactoryResetCountdown() {
    // Example: To be called directly with a value
}

