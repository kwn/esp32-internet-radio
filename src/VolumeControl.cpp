#include "VolumeControl.h"

VolumeControl::VolumeControl(Encoder* enc, Audio* aud, long initialVolume) {
    encoder = enc;
    audio = aud;
    currentVolume = initialVolume;
    audio->setVolume(currentVolume);
}

void VolumeControl::handleVolume() {
    long volumeChange = (int)(encoder->read() / 4);

    if (volumeChange != 0) {
        if (volumeIncreased(volumeChange)) {
            currentVolume = min(currentVolume + volumeChange, 21l);
        } else if (volumeDecreased(volumeChange)) {
            currentVolume = max(currentVolume + volumeChange, 0l);
        }

        audio->setVolume(currentVolume);
        encoder->write(0);

        Serial.println("Set volume to: " + String(currentVolume));
    }
}

bool VolumeControl::volumeDecreased(long newVolume) {
    return newVolume < 0;
}

bool VolumeControl::volumeIncreased(long newVolume) {
    return newVolume > 0;
}
