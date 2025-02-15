#ifndef TONE_CONTROL_H
#define TONE_CONTROL_H

#include <Arduino.h>
#include <Encoder.h>
#include <Audio.h>

class ToneControl {
private:
    Encoder* encoder;
    Audio* audio;
    int switchPin;
    int currentTone;
    int currentLowPass;
    int currentHighPass;
    static const int minGain = -8;
    static const int maxGain = 10;

    void updateTone();

public:
    ToneControl(Encoder* enc, Audio* aud, int encoderSwitchPin);
    void handleTone();
    void handleReset();
};

#endif // TONE_CONTROL_H
