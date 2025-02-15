#ifndef TONE_CONTROL_H
#define TONE_CONTROL_H

#include <Arduino.h>
#include <Encoder.h>
#include <Audio.h>
#include <AiEsp32RotaryEncoder.h>

#define TONE_CONTROL_MIN_BOUNDRY -8
#define TONE_CONTROL_MAX_BOUNDRY 10

class ToneControl {
private:
    AiEsp32RotaryEncoder* encoder;
    Audio* audio;

    void updateTone(int low, int high);

public:
    ToneControl(AiEsp32RotaryEncoder* enc, Audio* aud);
    void handleTone();
    void handleReset();
};

#endif // TONE_CONTROL_H
