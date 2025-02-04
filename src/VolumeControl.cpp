#include "VolumeControl.h"

VolumeControl::VolumeControl(Encoder* enc, Audio* aud, int initialVolume) {
    encoder = enc;
    audio = aud;
    currentVolume = initialVolume;

    audio->setVolume(currentVolume);
}

void VolumeControl::handleVolume() {
    int volumeChange = (int)(encoder->read() / 4);

    if (volumeIncreased(volumeChange)) {
        increaseVolume(volumeChange);
    } else if (volumeDecreased(volumeChange)) {
        decreaseVolume(volumeChange);
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
