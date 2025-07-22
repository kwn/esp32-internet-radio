#ifndef TONECONTROL_H
#define TONECONTROL_H

#include <Audio.h>
#include <AiEsp32RotaryEncoder.h>
#include <Preferences.h>
#include "LedControl.h" // Include LedControl

#define TONE_CONTROL_MIN_BOUNDRY -5
#define TONE_CONTROL_MAX_BOUNDRY 5
#define TONE_CONTROL_INITIAL_TONE 0
#define ROTARY_ENCODER_STEPS 4

class ToneControl {
private:
    AiEsp32RotaryEncoder* encoder;
    Audio* audio;
    Preferences* preferences;
    LedControl* ledControl; // Add LedControl pointer
    static ToneControl* instance;

    static void IRAM_ATTR readEncoderISR();
    void updateTone(int tone);

public:
    // Update constructor
    ToneControl(Audio* aud, Preferences* prefs, LedControl* ledCtrl, int pinCLK, int pinDT, int pinSW);
    void handleChange();
    void handleReset();
    int getTone();
};

#endif // TONE_CONTROL_H
