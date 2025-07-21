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
        case VOLUME_CHANGE:
            // These are transient and handled differently, maybe via direct calls.
            // For now, they can fall through or show a default state.
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
    const int tailLength = 3;
    // Animate from off-screen-right to off-screen-left to ensure tail is fully visible at start and end
    for (int i = numLeds + tailLength; i >= 0; i--) {
        clear();

        // Draw the bright head of the comet
        if (i < numLeds) {
            leds[i] = CRGB(20, 8, 0);
        }

        // Draw the fading tail
        for (int j = 1; j <= tailLength; j++) {
            int tailIndex = i + j;
            if (tailIndex < numLeds) {
                // Fade brightness with distance from head
                leds[tailIndex] = CRGB(20, 8, 0);
                leds[tailIndex].nscale8(255 - (j * (255 / (tailLength + 1))));
            }
        }

        FastLED.show();
        delay(30); // Adjust for desired speed
    }
    clear(); // Clear strip after animation
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
    fill_solid(leds, numLeds, CRGB(20, 8, 0));

    int station = stationControl->getStationNumber();
    int ledIndex = numLeds - 1 - station;

    // Calculate "breathing" brightness using a sine wave for a smoother, more visible pulse.
    // 60 BPM = 1 full sine wave cycle per second.
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
    FastLED.setBrightness(40);
}

void LedControl::showVolumeChange(int volume) {
    // Example: To be called directly with a value
}

void LedControl::showFactoryResetCountdown() {
    // Example: To be called directly with a value
}

