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

StationControl* StationControl::instance = nullptr;

StationControl::StationControl(Audio* aud, int pinCLK, int pinDT, int pinSW):
    audio(aud) {
    instance = this;
    encoder = new AiEsp32RotaryEncoder(pinCLK, pinDT, pinSW, -1, ROTARY_ENCODER_STEPS);
    encoder->begin();
    encoder->setup(readEncoderISR);
    encoder->setBoundaries(STATION_CONTROL_MIN_BOUNDRY, STATION_CONTROL_MAX_BOUNDRY, false);
    encoder->disableAcceleration();
    encoder->setEncoderValue(STATION_CONTROL_INITIAL_STATION);
}

void IRAM_ATTR StationControl::readEncoderISR() {
    if (instance) {
        instance->encoder->readEncoder_ISR();
    }
}

void StationControl::handleStationChange() {
    if (encoder->encoderChanged()) {
        Serial.println("StationControl: Set station to " + String(stations[encoder->readEncoder()]));

        reconnect();
    }
}

void StationControl::reconnect() {
    audio->connecttohost(stations[encoder->readEncoder()]);
}
