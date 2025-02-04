#ifndef STATIONCONTROL_H
#define STATIONCONTROL_H

#include <Encoder.h>
#include <Audio.h>

class StationControl {
private:
    Encoder* encoder;
    Audio* audio;
    int currentStation;
    static const char* stations[];
    static const int numStations;

public:
    StationControl(Encoder* enc, Audio* aud, int initialStation = 0);
    void handleStationChange();
    const char* getCurrentStationUrl() const;
};

#endif
