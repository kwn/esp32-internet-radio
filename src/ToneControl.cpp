#include "ToneControl.h"

ToneControl::ToneControl(Encoder* enc, Audio* aud, int encoderSwitchPin)
    : encoder(enc), audio(aud), switchPin(encoderSwitchPin),
      currentTone(0), currentLowPass(0), currentHighPass(0) {}

void ToneControl::handleTone() {
    int position = (int)(encoder->read() / 4);

    if (position != 0) {
        Serial.printf("position: %d\n", position);

        if (position < 0 && currentTone > minGain) {
            currentTone = max(currentTone - 1, minGain);
        } else if (position > 0 && currentTone < maxGain) {
            currentTone = min(currentTone + 1, maxGain);
        }

        if (currentTone < 0) {
            currentLowPass = abs(currentTone);
            currentHighPass = max(-abs(currentTone) / 2, minGain);
        } else if (currentTone > 0) {
            currentHighPass = currentTone;
            currentLowPass = max(-currentTone / 2, minGain);
        } else {
            currentLowPass = 0;
            currentHighPass = 0;
        }

        updateTone();
    }
}

void ToneControl::updateTone() {
    audio->setTone(currentLowPass, 0, currentHighPass);
    encoder->write(0);

    Serial.printf("Tone: %d, Bass: %d, Treble: %d\n", currentTone, currentLowPass, currentHighPass);
}

void ToneControl::handleReset() {
    static bool buttonPressed = false;

    if (digitalRead(switchPin) == LOW) {
        if (!buttonPressed) {
            buttonPressed = !buttonPressed;
            currentLowPass = 0;
            currentHighPass = 0;
            currentTone = 0;

            updateTone();

            Serial.println("Tone reset to neutral");
        }
    } else {
        buttonPressed = false;
    }
}
