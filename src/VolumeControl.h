#ifndef VOLUMECONTROL_H
#define VOLUMECONTROL_H

#include <Audio.h>
#include <AiEsp32RotaryEncoder.h>
#include <Preferences.h>

#define VOLUME_CONTROL_MIN_BOUNDRY 0
#define VOLUME_CONTROL_MAX_BOUNDRY 21
#define VOLUME_CONTROL_INITIAL_VOLUME 5
#define ROTARY_ENCODER_STEPS 4

class VolumeControl {
private:
    AiEsp32RotaryEncoder* encoder;
    Audio* audio;
    Preferences* preferences;
    static VolumeControl* instance;
    bool isMuted;
    int muteVolume;

    static void IRAM_ATTR readEncoderISR();
    void updateVolume(int volume);

public:
    VolumeControl(Audio* aud, Preferences* prefs, int pinCLK, int pinDT, int pinSW);
    void handleChange();
    void handleMute();
};

#endif
