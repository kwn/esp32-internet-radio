#include <Audio.h>
#include <AiEsp32RotaryEncoder.h>
#include <esp_bt.h>

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
#define PIN_ENCODER3_CLK 17
#define PIN_ENCODER3_DT 16
#define PIN_ENCODER3_SW 4

Audio audio;
Preferences preferences;

VolumeControl *volumeControl;
StationControl *stationControl;
WiFiControl *wifiControl;
LedControl *ledControl;
ToneControl *toneControl;

void setup() {
    Serial.begin(9600);
    Serial.println("Main: Starting setup...");

    esp_bt_controller_disable();

    pinMode(PIN_LED_RED, OUTPUT);
    pinMode(PIN_LED_GREEN, OUTPUT);
    pinMode(PIN_LED_BLUE, OUTPUT);
    pinMode(PIN_I2S_BCLK, OUTPUT);
    pinMode(PIN_I2S_DOUT, OUTPUT);
    pinMode(PIN_I2S_LRC, OUTPUT);
    pinMode(PIN_ENCODER1_DT, INPUT);
    pinMode(PIN_ENCODER1_CLK, INPUT);
    pinMode(PIN_ENCODER1_SW, INPUT);
    pinMode(PIN_ENCODER2_DT, INPUT);
    pinMode(PIN_ENCODER2_CLK, INPUT);
    pinMode(PIN_ENCODER2_SW, INPUT);
    pinMode(PIN_ENCODER3_DT, INPUT);
    pinMode(PIN_ENCODER3_CLK, INPUT);
    pinMode(PIN_ENCODER3_SW, INPUT);

    Serial.println("Main: Setting up audio...");

    audio.setPinout(PIN_I2S_BCLK, PIN_I2S_LRC, PIN_I2S_DOUT);
    audio.setBufsize(24 * 1024, -1);
    audio.forceMono(true);

    stationControl = new StationControl(&audio, PIN_ENCODER1_CLK, PIN_ENCODER1_DT, PIN_ENCODER1_SW);
    wifiControl = new WiFiControl(preferences);
    ledControl = new LedControl(PIN_LED_RED, PIN_LED_GREEN, PIN_LED_BLUE);
    volumeControl = new VolumeControl(&audio, PIN_ENCODER3_CLK, PIN_ENCODER3_DT, PIN_ENCODER3_SW);
    toneControl = new ToneControl(&audio, PIN_ENCODER2_CLK, PIN_ENCODER2_DT, PIN_ENCODER2_SW);

    ledControl->setColour(COLOUR_RED);
    wifiControl->setupWiFi();
}

void loop() {
    if (wifiControl->isConnected()) {
        volumeControl->handleChange();
        volumeControl->handleMute();
        toneControl->handleChange();
        toneControl->handleReset();
        stationControl->handleStationChange();
        wifiControl->displayWiFiSignalStrength();

        if (audio.isRunning()) {
            audio.loop();
            ledControl->setColour(COLOUR_GREEN);
        } else {
            Serial.println("Main: Audio stopped or failed, attempting to reconnect...");
            delay(2000);
            stationControl->reconnect();
        }
    } else {
        Serial.println("Main: Wifi not connected...");

        ledControl->setColour(COLOUR_BLUE);
        delay(2000);

        wifiControl->reconnect();
    }
}

void audio_info(const char *info) {
    // Serial.println("O GURWA!");
    Serial.println("Main: Audio info: " + String(info));
}

void audio_id3data(const char *info) {
    // Serial.println("CO TO GURWA JEST?!");
    Serial.println("Main: ID3 info: " + String(info));
}

void audio_showstreamtitle(const char *info) {
    // Serial.println("CZYMAJCIE MNIE GURWA!");
    Serial.println("Main: Stream info: " + String(info));
}
