#include "ToneControl.h"

ToneControl* ToneControl::instance = nullptr;

// Update constructor to accept LedControl
ToneControl::ToneControl(Audio* aud, Preferences* prefs, LedControl* ledCtrl, int pinCLK, int pinDT, int pinSW):
    audio(aud), preferences(prefs), ledControl(ledCtrl) {
    instance = this;

    int initialTone = preferences->getInt("tone", TONE_CONTROL_INITIAL_TONE);
    updateTone(initialTone);

    encoder = new AiEsp32RotaryEncoder(pinCLK, pinDT, pinSW, -1, ROTARY_ENCODER_STEPS);
    encoder->begin();
    encoder->setup(readEncoderISR);
    encoder->setBoundaries(TONE_CONTROL_MIN_BOUNDRY, TONE_CONTROL_MAX_BOUNDRY, false);
    encoder->disableAcceleration();
    encoder->setEncoderValue(initialTone);
}

void IRAM_ATTR ToneControl::readEncoderISR() {
    if (instance) {
        instance->encoder->readEncoder_ISR();
    }
}

void ToneControl::handleChange() {
    if (encoder->encoderChanged()) {
        int encoderValue = encoder->readEncoder();
        Serial.printf("ToneControl: Tone changed: %d\n", encoderValue);
        updateTone(encoderValue);
        ledControl->triggerToneOverlay(encoderValue); // Trigger tone overlay
    }
}

void ToneControl::updateTone(int tone) {
    preferences->putInt("tone", tone);

    int lowOffset = 2;
    int highOffset = 3;
    int lowAdj = 0;
    int highAdj = 0;

    // Symmetrical adjustment: for every point of tone, adjust bass/treble by a half point.
    if (tone < 0) { // More bass
        lowAdj = abs(tone);
        highAdj = (int)(tone * 0.5); // Reduce treble
    } else if (tone > 0) { // More treble
        lowAdj = (int)(-tone * 0.5); // Reduce bass
        highAdj = tone;
    }

    int finalLow = lowAdj + lowOffset;
    int finalHigh = highAdj + highOffset;

    audio->setTone(finalLow, 0, finalHigh);

    Serial.printf("ToneControl: Bass: %d, Treble: %d\n", finalLow, finalHigh);
}

void ToneControl::handleReset() {
    static bool buttonPressed = false;

    if (encoder->isEncoderButtonDown() && !buttonPressed) {
        buttonPressed = true;
        encoder->setEncoderValue(0);
        updateTone(0);

        Serial.println("ToneControl: Tone reset to neutral");
    } else if (!encoder->isEncoderButtonDown()) {
        buttonPressed = false;
    }
}

int ToneControl::getTone() {
    return encoder->readEncoder();
}
