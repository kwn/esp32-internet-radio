#ifndef TONE_CONTROL_H
#define TONE_CONTROL_H

#include <Audio.h>
#include <AiEsp32RotaryEncoder.h>

#define TONE_CONTROL_MIN_BOUNDRY -8
#define TONE_CONTROL_MAX_BOUNDRY 10
#define ROTARY_ENCODER_STEPS 4

class ToneControl {
private:
    AiEsp32RotaryEncoder* encoder;
    Audio* audio;
    static ToneControl* instance;

    static void IRAM_ATTR readEncoderISR();
    void updateTone(int low, int high);

public:
    ToneControl(Audio* aud, int pinCLK, int pinDT, int pinSW);
    void handleChange();
    void handleReset();
};

#endif // TONE_CONTROL_H
