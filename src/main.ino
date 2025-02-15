#include <Audio.h>
#include <Encoder.h>
#include <AiEsp32RotaryEncoder.h>

#include "VolumeControl.h"
#include "StationControl.h"
#include "WiFiControl.h"
#include "LedControl.h"
#include "ToneControl.h"

#define PIN_LED_RED 21
#define PIN_LED_GREEN 22
#define PIN_LED_BLUE 23
#define PIN_I2S_DOUT 26
#define PIN_I2S_BCLK 27
#define PIN_I2S_LRC 25
#define PIN_ENCODER1_CLK 5
#define PIN_ENCODER1_DT 18
#define PIN_ENCODER1_SW 19
#define PIN_ENCODER2_CLK 35
#define PIN_ENCODER2_DT 32
#define PIN_ENCODER2_SW 33
#define ROTARY_ENCODER_STEPS 4

Audio audio;
Preferences preferences;
Encoder *encoder1;
Encoder *encoder2;
AiEsp32RotaryEncoder rotaryEncoder = AiEsp32RotaryEncoder(PIN_ENCODER2_CLK, PIN_ENCODER2_DT, PIN_ENCODER2_SW, -1, ROTARY_ENCODER_STEPS);

VolumeControl *volumeControl;
StationControl *stationControl;
WiFiControl *wifiControl;
LedControl *ledControl;
ToneControl *toneControl;

void IRAM_ATTR readEncoderISR() {
    rotaryEncoder.readEncoder_ISR();
}

void setup() {
    Serial.begin(9600);
    Serial.println("Starting setup...");

    pinMode(PIN_LED_RED, OUTPUT);
    pinMode(PIN_LED_GREEN, OUTPUT);
    pinMode(PIN_LED_BLUE, OUTPUT);
    pinMode(PIN_I2S_BCLK, OUTPUT);
    pinMode(PIN_I2S_DOUT, OUTPUT);
    pinMode(PIN_I2S_LRC, OUTPUT);
    pinMode(PIN_ENCODER1_DT, INPUT);
    pinMode(PIN_ENCODER1_CLK, INPUT);
    pinMode(PIN_ENCODER1_SW, INPUT_PULLUP); // doesn't have pull up resistor for switch
    pinMode(PIN_ENCODER2_DT, INPUT);
    pinMode(PIN_ENCODER2_CLK, INPUT);
    pinMode(PIN_ENCODER2_SW, INPUT);

    encoder1 = new Encoder(PIN_ENCODER1_CLK, PIN_ENCODER1_DT);

    rotaryEncoder.begin();
    rotaryEncoder.setup(readEncoderISR);
    rotaryEncoder.setBoundaries(TONE_CONTROL_MIN_BOUNDRY, TONE_CONTROL_MAX_BOUNDRY, false);
    rotaryEncoder.disableAcceleration();


    // volumeControl = new VolumeControl(encoder2, &audio, PIN_ENCODER2_SW);
    stationControl = new StationControl(encoder1, &audio);
    wifiControl = new WiFiControl(preferences);
    ledControl = new LedControl(PIN_LED_RED, PIN_LED_GREEN, PIN_LED_BLUE);
    toneControl = new ToneControl(&rotaryEncoder, &audio);

    ledControl->setColour(COLOUR_RED);

    Serial.println("Setting up audio...");

    audio.setPinout(PIN_I2S_BCLK, PIN_I2S_LRC, PIN_I2S_DOUT);
    audio.forceMono(true);

    wifiControl->setupWiFi();
}

void loop() {
    // volumeControl->handleVolume();
    // volumeControl->handleMute();
    toneControl->handleTone();
    toneControl->handleReset();
    stationControl->handleStationChange();

    if (wifiControl->isConnected()) {
        if (audio.isRunning()) {
            audio.loop();
            ledControl->setColour(COLOUR_GREEN);
        } else {
            Serial.println("Audio stopped or failed, attempting to reconnect...");
            delay(2000);
            stationControl->reconnect();
        }
    } else {
        Serial.println("Wifi not connected...");

        ledControl->setColour(COLOUR_BLUE);
        delay(5000);

        wifiControl->reconnect();
    }
}

void audio_info(const char *info) {
    Serial.println("O GURWA!");

    String stringInfo = String(info);
    Serial.println(stringInfo);
}

void audio_id3data(const char *info) {
    Serial.println("CO TO GURWA JEST?!");

    // String sinfo = String(info);
    Serial.println(String(info));
}

void audio_showstreamtitle(const char *info) {
    Serial.println("CZYMAJCIE MNIE GURWA!");
    // String sinfo = String(info);
    Serial.println(String(info));
}
