#include "VolumeControl.h"

VolumeControl::VolumeControl(Encoder* enc, Audio* aud, int encoderSwitchPin, int initialVolume) {
    encoder = enc;
    audio = aud;
    currentVolume = initialVolume;
    isMuted = false;
    switchPin = encoderSwitchPin;

    audio->setVolume(currentVolume);
}

void VolumeControl::handleVolume() {
    if (!isMuted) {
        int volumeChange = (int)(encoder->read() / 4);

        if (volumeIncreased(volumeChange)) {
            increaseVolume(volumeChange);
        } else if (volumeDecreased(volumeChange)) {
            decreaseVolume(volumeChange);
        }
    }
}

bool VolumeControl::volumeDecreased(int newVolume) {
    return newVolume < 0;
}

bool VolumeControl::volumeIncreased(int newVolume) {
    return newVolume > 0;
}

void VolumeControl::increaseVolume(int volumeChange) {
    currentVolume = min(currentVolume + volumeChange, 21);

    audio->setVolume(currentVolume);
    encoder->write(0);

    Serial.println("Volume increased to " + String(currentVolume));
}

void VolumeControl::decreaseVolume(int volumeChange) {
    currentVolume = max(currentVolume + volumeChange, 0);

    audio->setVolume(currentVolume);
    encoder->write(0);

    Serial.println("Volume decreased to " + String(currentVolume));
}

void VolumeControl::handleMute() {
    static bool buttonPressed = false;

    if (digitalRead(switchPin) == LOW) {
        if (!buttonPressed) {
            isMuted = !isMuted;
            buttonPressed = true;

            if (isMuted) {
                audio->setVolume(0);
                Serial.println("Muted");
            } else {
                audio->setVolume(currentVolume);
                Serial.println("Unmuted");
            }
        }
    } else {
        buttonPressed = false;
    }
}
