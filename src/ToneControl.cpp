#include "ToneControl.h"

ToneControl* ToneControl::instance = nullptr;

const int8_t basssMap[11] = {
    6, 4, 3, 2, 1, /**/0/**/, -2, -4, -6, -8, -10
};

const int8_t trebleMap[11] = {
    -10, -8, -6, -4, -2, /**/0/**/, 1, 2, 3, 4, 6
};

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

        updateTone(encoderValue);
        ledControl->triggerToneOverlay(encoderValue);

        Serial.printf("ToneControl: Tone changed: %d\n", encoderValue);
    }
}

void ToneControl::updateTone(int tone) {
    preferences->putInt("tone", tone);

    audio->setTone(basssMap[tone], 0, trebleMap[tone]);

    Serial.printf("ToneControl: Bass: %d, Treble: %d\n", basssMap[tone], trebleMap[tone]);
}

void ToneControl::handleReset() {
    static bool buttonPressed = false;

    if (encoder->isEncoderButtonDown() && !buttonPressed) {
        buttonPressed = true;
        encoder->setEncoderValue(TONE_CONTROL_INITIAL_TONE);
        ledControl->triggerToneOverlay(TONE_CONTROL_INITIAL_TONE);

        updateTone(TONE_CONTROL_INITIAL_TONE);

        Serial.println("ToneControl: Tone reset to neutral");
    } else if (!encoder->isEncoderButtonDown()) {
        buttonPressed = false;
    }
}

int ToneControl::getTone() {
    return encoder->readEncoder();
}
