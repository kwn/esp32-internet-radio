#include "ToneControl.h"

ToneControl* ToneControl::instance = nullptr;

ToneControl::ToneControl(Audio* aud, int pinCLK, int pinDT, int pinSW):
    audio(aud) {
    instance = this;
    encoder = new AiEsp32RotaryEncoder(pinCLK, pinDT, pinSW, -1, ROTARY_ENCODER_STEPS);
    encoder->begin();
    encoder->setup(readEncoderISR);
    encoder->setBoundaries(TONE_CONTROL_MIN_BOUNDRY, TONE_CONTROL_MAX_BOUNDRY, false);
    encoder->disableAcceleration();
}

void IRAM_ATTR ToneControl::readEncoderISR() {
    if (instance) {
        instance->encoder->readEncoder_ISR();
    }
}

void ToneControl::handleChange() {
    if (encoder->encoderChanged()) {
        int encoderValue = encoder->readEncoder();
        int low = 0;
        int high = 0;

        Serial.printf("ToneControl: Tone changed: %d\n", encoderValue);

        if (encoderValue < 0) {
            low = abs(encoderValue);
            high = (int)(encoderValue / 2);
        } else if (encoderValue > 0) {
            low = -abs(encoderValue / 2);
            high = encoderValue;
        }

        updateTone(low, high);
    }
}

void ToneControl::updateTone(int low, int high) {
    audio->setTone(low, 0, high);

    Serial.printf("ToneControl: Bass: %d, Treble: %d\n", low, high);
}

void ToneControl::handleReset() {
    static bool buttonPressed = false;

    if (encoder->isEncoderButtonDown() && !buttonPressed) {
        buttonPressed = true;
        encoder->setEncoderValue(0);
        updateTone(0, 0);

        Serial.println("ToneControl: Tone reset to neutral");
    } else if (!encoder->isEncoderButtonDown()) {
        buttonPressed = false;
    }
}
