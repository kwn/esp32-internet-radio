#include "VolumeControl.h"

extern float volumeAttenuation;

VolumeControl* VolumeControl::instance = nullptr;

VolumeControl::VolumeControl(Audio* aud, Preferences* prefs, StatusControl* statCtrl, LedControl* ledCtrl, int pinCLK, int pinDT, int pinSW):
    audio(aud), preferences(prefs), statusControl(statCtrl), ledControl(ledCtrl) {
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
    if (encoder->encoderChanged()) {
        int encoderValue = encoder->readEncoder();

        updateVolume(encoderValue);
        ledControl->triggerVolumeOverlay(getVolume());

        Serial.printf("VolumeControl: Volume changed: %d\n", encoderValue);
    }
}

void VolumeControl::updateVolume(int volume) {
    preferences->putInt("volume", volume);

    if (!statusControl->isMuted()) {
        // Map encoder value (0-11) to an attenuation range of 0% to 50%.
        // 50% (0.5) corresponds to the library's level 11.
        // This gives us a much quieter and more granular volume curve.
        volumeAttenuation = map(volume, 0, 11, 0, 50) / 100.0f;
    }

    muteVolume = volume;
    Serial.printf("VolumeControl: Volume set to %d (Attenuation: %.1f%%)\n", volume, volumeAttenuation * 100);
}

void VolumeControl::handleMute() {
    static bool buttonPressed = false;

    if (encoder->isEncoderButtonDown() && !buttonPressed) {
        bool newMuteState = !statusControl->isMuted();
        statusControl->setMuted(newMuteState);
        buttonPressed = true;

        if (newMuteState) {
            volumeAttenuation = 0.0;
        } else {
            volumeAttenuation = map(muteVolume, 0, 11, 0, 50) / 100.0f;
        }

        encoder->setEncoderValue(newMuteState ? 0 : muteVolume);
    } else if (!encoder->isEncoderButtonDown()) {
        buttonPressed = false;
    }
}

int VolumeControl::getVolume() {
    return muteVolume;
}
