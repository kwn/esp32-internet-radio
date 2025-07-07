#include "LedControl.h"

LedControl::LedControl(CRGB* leds, int numLeds, StatusControl* statusControl, StationControl* stationControl)
    : leds(leds), numLeds(numLeds), statusControl(statusControl), stationControl(stationControl), animationStep(0), direction(1), animationDebouncer(60) {}

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
    fill_solid(leds, numLeds, CRGB::Green);
    FastLED.show();
    delay(500); // Brief flash
    clear();
    FastLED.show();
}

void LedControl::showWifiConnecting() {
    if (animationDebouncer.hasElapsed()) { // Controls the speed of the scanner
        clear();
        
        // Draw the scanner head (brightest). HUE_BLUE is 160.
        leds[animationStep] = CHSV(160, 255, 255);

        // Draw the gradient tail using a loop.
        for (int i = 1; i <= 3; i++) {
            int tailIndex = animationStep - (i * direction);
            if (tailIndex >= 0 && tailIndex < numLeds) {
                // Decrease brightness for each segment of the tail.
                leds[tailIndex] = CHSV(160, 255, 255 - (i * 60));
            }
        }

        // Check for boundaries and reverse direction *before* moving the scanner
        if (animationStep >= numLeds - 1) {
            direction = -1;
        } else if (animationStep <= 0) {
            direction = 1;
        }

        // Move the scanner head for the next frame
        animationStep += direction;
    }
}

void LedControl::showStreamBuffering() {
    // Get the station LED index (reversed).
    int station = stationControl->getStationNumber();
    int ledIndex = numLeds - 1 - station;

    // Calculate "breathing" brightness using a sine wave for a smoother, more visible pulse.
    // 60 BPM = 1 full sine wave cycle per second.
    uint8_t brightness = beatsin8(60, 0, 255);

    // Clear the strip first.
    clear();

    // Set the specific LED with the calculated brightness using HSV for better control.
    if (ledIndex >= 0 && ledIndex < numLeds) {
        // HUE_ORANGE is ~30. We directly control the Value (brightness) from 0-255.
        leds[ledIndex] = CHSV(30, 255, brightness);
    }
}

void LedControl::showPlaying() {
    clear();
    int station = stationControl->getStationNumber();
    int ledIndex = numLeds - 1 - station;

    if (ledIndex >= 0 && ledIndex < numLeds) {
        leds[ledIndex] = CRGB::Orange;
    }
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

