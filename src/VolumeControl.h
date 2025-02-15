#ifndef VOLUMECONTROL_H
#define VOLUMECONTROL_H

#include <Encoder.h>
#include <Audio.h>

class VolumeControl {
private:
    Encoder* encoder;
    Audio* audio;
    int currentVolume;
    bool isMuted;
    int switchPin;

    bool volumeDecreased(int newVolume);
    bool volumeIncreased(int newVolume);
    void increaseVolume(int volumeChange);
    void decreaseVolume(int volumeChange);

public:
    VolumeControl(Encoder* enc, Audio* aud, int encoderSwitchPin, int initialVolume = 21);
    void handleVolume();
    void handleMute();
};

#endif
