#ifndef VOLUMECONTROL_H
#define VOLUMECONTROL_H

#include <Audio.h>
#include <AiEsp32RotaryEncoder.h>

#define VOLUME_CONTROL_MIN_BOUNDRY 0
#define VOLUME_CONTROL_MAX_BOUNDRY 21
#define VOLUME_CONTROL_INITIAL_VOLUME 21
#define ROTARY_ENCODER_STEPS 4

class VolumeControl {
private:
    AiEsp32RotaryEncoder* encoder;
    Audio* audio;
    static VolumeControl* instance;
    bool isMuted;
    int muteVolume;

    static void IRAM_ATTR readEncoderISR();
    void updateVolume(int volume);

public:
    VolumeControl(Audio* aud, int pinCLK, int pinDT, int pinSW);
    void handleChange();
    void handleMute();
};

#endif
