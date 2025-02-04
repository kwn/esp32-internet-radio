#ifndef VOLUMECONTROL_H
#define VOLUMECONTROL_H

#include <Encoder.h>
#include <Audio.h>

class VolumeControl {
private:
    Encoder* encoder;
    Audio* audio;
    long currentVolume;

    bool volumeDecreased(long newVolume);
    bool volumeIncreased(long newVolume);

public:
    VolumeControl(Encoder* enc, Audio* aud, long initialVolume = 8);
    void handleVolume();
};

#endif
