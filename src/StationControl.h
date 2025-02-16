#ifndef STATIONCONTROL_H
#define STATIONCONTROL_H

#include <Audio.h>
#include <AiEsp32RotaryEncoder.h>

#define STATION_CONTROL_MIN_BOUNDRY 0
#define STATION_CONTROL_MAX_BOUNDRY 6
#define STATION_CONTROL_INITIAL_STATION 0
#define ROTARY_ENCODER_STEPS 4

class StationControl {
private:
    AiEsp32RotaryEncoder* encoder;
    Audio* audio;
    static StationControl* instance;
    static const char* stations[];


    static void IRAM_ATTR readEncoderISR();

public:
    StationControl(Audio* aud, int pinCLK, int pinDT, int pinSW);
    void handleStationChange();
    void reconnect();
};

#endif
