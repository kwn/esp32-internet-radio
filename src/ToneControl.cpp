#include "ToneControl.h"

ToneControl* ToneControl::instance = nullptr;

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

    int lowOffset = 2;
    int highOffset = 3;
    int lowAdj = 0;
    int highAdj = 0;

    if (tone < 0) {
        lowAdj = abs(tone);
        highAdj = -abs(tone);
    } else if (tone > 0) {
        lowAdj = -abs(tone);
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
        ledControl->triggerToneOverlay(0);

        updateTone(0);

        Serial.println("ToneControl: Tone reset to neutral");
    } else if (!encoder->isEncoderButtonDown()) {
        buttonPressed = false;
    }
}

int ToneControl::getTone() {
    return encoder->readEncoder();
}
