#include "ToneControl.h"

ToneControl::ToneControl(AiEsp32RotaryEncoder* enc, Audio* aud)
    : encoder(enc), audio(aud) {}

void ToneControl::handleTone() {
    if (encoder->encoderChanged()) {
        int encoderValue = encoder->readEncoder();

        Serial.printf("Tone control changed: %d\n", encoderValue);

        int low = 0;
        int high = 0;

        if (encoderValue < 0) {
            low = abs(encoderValue);
            high = (int)(encoderValue / 2);
        } else if (encoderValue > 0) {
            low = -abs((int)(encoderValue / 2));
            high = encoderValue;
        }

        updateTone(low, high);
    }
}

void ToneControl::updateTone(int low, int high) {
    audio->setTone(low, 0, high);

    Serial.printf("Bass: %d, Treble: %d\n", low, high);
}

void ToneControl::handleReset() {
    static bool buttonPressed = false;

    if (encoder->isEncoderButtonClicked()) {
        if (!buttonPressed) {
            buttonPressed = !buttonPressed;
            encoder->setEncoderValue(0);
            updateTone(0, 0);

            Serial.println("Tone reset to neutral");
        }
    } else {
        buttonPressed = false;
    }
}
