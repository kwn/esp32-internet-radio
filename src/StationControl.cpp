#include "StationControl.h"

StationControl::StationControl(Encoder* enc, Audio* aud, const char** stat, int numStat) {
    encoder = enc;
    audio = aud;
    stations = stat;
    numStations = numStat;
    currentStation = 0;
}

void StationControl::handleStationChange() {
    long newStation = (int)(encoder->read() / 4);
    if (newStation != 0) {
        if (newStation > 0) {
            currentStation = min(currentStation + newStation, numStations - 1);
        } else if (newStation < 0) {
            currentStation = max(currentStation + newStation, 0);
        }
        audio->connecttohost(stations[currentStation]);
        encoder->write(0);
        Serial.println("Set station to: " + String(currentStation));
    }
}
