#ifndef STATIONCONTROL_H
#define STATIONCONTROL_H

#include <Encoder.h>
#include <Audio.h>

class StationControl {
private:
    Encoder* encoder;
    Audio* audio;
    const char** stations;
    int numStations;
    int currentStation;

public:
    StationControl(Encoder* enc, Audio* aud, const char** stat, int numStat);
    void handleStationChange();
};

#endif
