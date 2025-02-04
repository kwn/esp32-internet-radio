#include "StationControl.h"

const char* StationControl::stations[] = {
  "https://fr4.1mix.co.uk:8000/32aac",
  "https://zt02.cdn.eurozet.pl/ZETHIT.mp3",
  "http://uk1.internet-radio.com:8294/live",
  "http://uk7.internet-radio.com:8226/live",
  "https://195.150.20.7/rmf_fm",
  "https://s3.slotex.pl:7046/stream/",
  "https://ssl-1.radiohost.pl:9680/stream"
};
const int StationControl::numStations = sizeof(StationControl::stations) / sizeof(StationControl::stations[0]);

StationControl::StationControl(Encoder* enc, Audio* aud, int initialStation) {
    encoder = enc;
    audio = aud;
    currentStation = initialStation;
}

void StationControl::handleStationChange() {
    int newStation = (int)(encoder->read() / 4);

    if (newStation != 0) {
        currentStation = max(0, min(currentStation + newStation, numStations - 1));

        reconnect();

        Serial.println("Set station to " + String(currentStation));
    }
}

void StationControl::reconnect() {
    audio->connecttohost(stations[currentStation]);
    encoder->write(0);
}
