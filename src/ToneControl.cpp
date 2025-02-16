#include "ToneControl.h"

ToneControl* ToneControl::instance = nullptr;

ToneControl::ToneControl(Audio* aud, Preferences* prefs, int pinCLK, int pinDT, int pinSW):
    audio(aud), preferences(prefs) {
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
    }
}

void ToneControl::updateTone(int tone) {
    preferences->putInt("tone", tone);

    int low = 0;
    int high = 0;

    if (tone < 0) {
        low = abs(tone);
        high = (int)(tone / 2);
    } else if (tone > 0) {
        low = -abs(tone / 2);
        high = tone;
    }

    audio->setTone(low, 0, high);

    Serial.printf("ToneControl: Bass: %d, Treble: %d\n", low, high);
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
