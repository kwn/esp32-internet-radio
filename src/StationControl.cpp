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

StationControl::StationControl(Audio* aud, Preferences* prefs, StatusControl* statCtrl, int pinCLK, int pinDT, int pinSW):
    audio(aud), preferences(prefs), statusControl(statCtrl) {
    instance = this;

    int initialStation = preferences->getInt("station", STATION_CONTROL_INITIAL_STATION);

    encoder = new AiEsp32RotaryEncoder(pinCLK, pinDT, pinSW, -1, ROTARY_ENCODER_STEPS);
    encoder->begin();
    encoder->setup(readEncoderISR);
    encoder->setBoundaries(STATION_CONTROL_MIN_BOUNDRY, STATION_CONTROL_MAX_BOUNDRY, false);
    encoder->disableAcceleration();
    encoder->setEncoderValue(initialStation);
}

void IRAM_ATTR StationControl::readEncoderISR() {
    if (instance) {
        instance->encoder->readEncoder_ISR();
    }
}

void StationControl::handleStationChange() {
    if (encoder->encoderChanged()) {
        int station = encoder->readEncoder();
        preferences->putInt("station", station);

        audio->stopSong();

        Serial.println("StationControl: Set station to " + String(stations[station]));
    }
}

void StationControl::reconnect() {
    audio->connecttohost(stations[encoder->readEncoder()]);
}

int StationControl::getStationNumber() {
    return encoder->readEncoder();
}

void StationControl::handleFactoryReset() {
    static unsigned long buttonPressStart = 0;
    static int lastCountdown = -1;

    if (encoder->isEncoderButtonDown()) {
        if (!statusControl->isFactoryResetting()) {
            buttonPressStart = millis();
            statusControl->setFactoryReset(true);
        }

        unsigned long elapsed = millis() - buttonPressStart;
        int remainingTime = 5 - (elapsed / 1000);

        if (remainingTime != lastCountdown && remainingTime >= 0) {
            Serial.printf("Factory reset in %d seconds...\n", remainingTime);
            lastCountdown = remainingTime;
        }

        if (elapsed >= 5000) {
            Serial.println("Factory reset now!");
            preferences->clear();
            ESP.restart();
        }
    } else {
        if (statusControl->isFactoryResetting()) {
            statusControl->setFactoryReset(false);
        }
        lastCountdown = -1;
    }
}

