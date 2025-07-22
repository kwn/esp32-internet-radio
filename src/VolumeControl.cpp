#include "VolumeControl.h"

VolumeControl* VolumeControl::instance = nullptr;

// Update constructor to accept StatusControl
VolumeControl::VolumeControl(Audio* aud, Preferences* prefs, StatusControl* statCtrl, int pinCLK, int pinDT, int pinSW):
    audio(aud), preferences(prefs), statusControl(statCtrl) {
    instance = this;

    int initialVolume = preferences->getInt("volume", VOLUME_CONTROL_INITIAL_VOLUME);
    updateVolume(initialVolume);

    encoder = new AiEsp32RotaryEncoder(pinCLK, pinDT, pinSW, -1, ROTARY_ENCODER_STEPS);
    encoder->begin();
    encoder->setup(readEncoderISR);
    encoder->setBoundaries(VOLUME_CONTROL_MIN_BOUNDRY, VOLUME_CONTROL_MAX_BOUNDRY, false);
    encoder->disableAcceleration();
    encoder->setEncoderValue(initialVolume);
}

void IRAM_ATTR VolumeControl::readEncoderISR() {
    if (instance) {
        instance->encoder->readEncoder_ISR();
    }
}

void VolumeControl::handleChange() {
    // No longer check isMuted here, handle volume changes regardless.
    if (encoder->encoderChanged()) {
        int encoderValue = encoder->readEncoder();
        Serial.printf("VolumeControl: Volume changed: %d\n", encoderValue);
        updateVolume(encoderValue);
    }
}

void VolumeControl::updateVolume(int volume) {
    preferences->putInt("volume", volume);
    // Only update hardware volume if not muted
    if (!statusControl->isMuted()) {
        audio->setVolume(volume);
    }
    muteVolume = volume;
    Serial.printf("VolumeControl: Volume set to: %d\n", volume);
}

void VolumeControl::handleMute() {
    static bool buttonPressed = false;

    if (encoder->isEncoderButtonDown() && !buttonPressed) {
        bool newMuteState = !statusControl->isMuted();
        statusControl->setMuted(newMuteState); // Update central status
        buttonPressed = true;

        audio->setVolume(newMuteState ? 0 : muteVolume);
        encoder->setEncoderValue(newMuteState ? 0 : muteVolume);
    } else if (!encoder->isEncoderButtonDown()) {
        buttonPressed = false;
    }
}
