#include "VolumeControl.h"

VolumeControl* VolumeControl::instance = nullptr;

VolumeControl::VolumeControl(Audio* aud, int pinCLK, int pinDT, int pinSW):
    audio(aud), isMuted(false), muteVolume(VOLUME_CONTROL_INITIAL_VOLUME) {
    instance = this;
    encoder = new AiEsp32RotaryEncoder(pinCLK, pinDT, pinSW, -1, ROTARY_ENCODER_STEPS);
    encoder->begin();
    encoder->setup(readEncoderISR);
    encoder->setBoundaries(VOLUME_CONTROL_MIN_BOUNDRY, VOLUME_CONTROL_MAX_BOUNDRY, false);
    encoder->disableAcceleration();
    encoder->setEncoderValue(VOLUME_CONTROL_INITIAL_VOLUME);

    audio->setVolume(VOLUME_CONTROL_INITIAL_VOLUME);
}

void IRAM_ATTR VolumeControl::readEncoderISR() {
    if (instance) {
        instance->encoder->readEncoder_ISR();
    }
}

void VolumeControl::handleChange() {
    if (!isMuted && encoder->encoderChanged()) {
        int encoderValue = encoder->readEncoder();

        Serial.printf("Volume changed: %d\n", encoderValue);

        updateVolume(encoderValue);
    }
}

void VolumeControl::updateVolume(int volume) {
    audio->setVolume(volume);
    muteVolume = volume;

    Serial.printf("Volume: %d\n", volume);
}

void VolumeControl::handleMute() {
    static bool buttonPressed = false;

    if (encoder->isEncoderButtonDown()) {
        if (!buttonPressed) {
            isMuted = !isMuted;
            buttonPressed = true;

            if (isMuted) {
                audio->setVolume(0);
                encoder->setEncoderValue(0);
                Serial.println("Muted");
            } else {
                audio->setVolume(muteVolume);
                encoder->setEncoderValue(muteVolume);
                Serial.println("Unmuted");
            }
        }
    } else {
        buttonPressed = false;
    }
}
