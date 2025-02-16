#ifndef STATIONCONTROL_H
#define STATIONCONTROL_H

#include <Audio.h>
#include <AiEsp32RotaryEncoder.h>
#include <Preferences.h>

#define STATION_CONTROL_MIN_BOUNDRY 0
#define STATION_CONTROL_MAX_BOUNDRY 6
#define STATION_CONTROL_INITIAL_STATION 0
#define ROTARY_ENCODER_STEPS 4

class StationControl {
private:
    AiEsp32RotaryEncoder* encoder;
    Audio* audio;
    Preferences* preferences;
    static StationControl* instance;
    static const char* stations[];


    static void IRAM_ATTR readEncoderISR();

public:
    StationControl(Audio* aud, Preferences* prefs, int pinCLK, int pinDT, int pinSW);
    void handleStationChange();
    void reconnect();
};

#endif
