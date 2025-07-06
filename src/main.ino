#include <Audio.h>
#include <AiEsp32RotaryEncoder.h>
#include <esp_bt.h>
#include <Preferences.h>
#include <FastLED.h>

#include "VolumeControl.h"
#include "StationControl.h"
#include "WiFiControl.h"
#include "LedControl.h"
#include "ToneControl.h"
#include "StatusControl.h"

#define PIN_I2S_DOUT 12
#define PIN_I2S_BCLK 13
#define PIN_I2S_LRC 14
#define PIN_ENCODER1_CLK 21 // Station
#define PIN_ENCODER1_DT 22
#define PIN_ENCODER1_SW 23
#define PIN_ENCODER2_CLK 5 // Tone
#define PIN_ENCODER2_DT 18
#define PIN_ENCODER2_SW 19
#define PIN_ENCODER3_CLK 15 // Volume
#define PIN_ENCODER3_DT 16
#define PIN_ENCODER3_SW 17
#define PIN_LED_DATA 27

#define LED_COLOR_ORDER GRB
#define LED_TYPE WS2812B
#define LED_NUMBER 11
#define LED_BRIGHTNESS 40

Audio audio;
Preferences preferences;
CRGB leds[LED_NUMBER];

VolumeControl *volumeControl;
StationControl *stationControl;
WiFiControl *wifiControl;
LedControl *ledControl;
ToneControl *toneControl;
StatusControl *statusControl;

void ledTask(void *pvParameters) {
    for (;;) {
        ledControl->update();
        // Delay for a short period to allow other tasks to run.
        // This effectively sets the animation's frame rate. 10ms = ~100 FPS.
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void setup() {
    Serial.begin(9600);
    Serial.println("Main: Starting setup...");

    esp_bt_controller_disable();
    preferences.begin("radio");

    pinMode(PIN_LED_DATA, OUTPUT);
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

    statusControl = new StatusControl();
    statusControl->setState(POWER_ON);

    Serial.println("Main: Setting up audio...");

    audio.setPinout(PIN_I2S_BCLK, PIN_I2S_LRC, PIN_I2S_DOUT);
    audio.setBufsize(24 * 1024, -1);
    audio.forceMono(true);

    FastLED.addLeds<LED_TYPE, PIN_LED_DATA, LED_COLOR_ORDER>(leds, LED_NUMBER);
    FastLED.setBrightness(LED_BRIGHTNESS);

    stationControl = new StationControl(&audio, &preferences, statusControl, PIN_ENCODER1_CLK, PIN_ENCODER1_DT, PIN_ENCODER1_SW);
    ledControl = new LedControl(leds, LED_NUMBER, statusControl, stationControl);
    wifiControl = new WiFiControl(&preferences);
    volumeControl = new VolumeControl(&audio, &preferences, PIN_ENCODER3_CLK, PIN_ENCODER3_DT, PIN_ENCODER3_SW);
    toneControl = new ToneControl(&audio, &preferences, PIN_ENCODER2_CLK, PIN_ENCODER2_DT, PIN_ENCODER2_SW);

    xTaskCreatePinnedToCore(
        ledTask,          // Task function
        "LED Update Task",// Name of the task
        2048,             // Stack size of task
        NULL,             // Parameter of the task
        1,                // Priority of the task
        NULL,             // Task handle
        1);               // Core ID

    statusControl->setState(WIFI_CONNECTING);
    wifiControl->setupWiFi();
}

void loop() {
    stationControl->handleFactoryReset();

    if (wifiControl->isConnected()) {
        volumeControl->handleChange();
        volumeControl->handleMute();
        toneControl->handleChange();
        toneControl->handleReset();
        stationControl->handleStationChange();
        // wifiControl->displayWiFiSignalStrength();

        if (audio.isRunning()) {
            statusControl->setState(PLAYING);
            audio.loop();
        } else {
            statusControl->setState(STREAM_BUFFERING);

            static unsigned long lastStreamReconnectAttempt = 0;
            unsigned long currentTime = millis();
            if (currentTime - lastStreamReconnectAttempt > 5000) {
                Serial.println("Main: Audio stopped or failed, attempting to reconnect...");
                lastStreamReconnectAttempt = currentTime;
                stationControl->reconnect();
            }
        }
    } else {
        statusControl->setState(WIFI_CONNECTING);

        static unsigned long lastReconnectAttempt = 0;
        unsigned long currentTime = millis();
        if (currentTime - lastReconnectAttempt > 5000) {
            Serial.println("Main: Wifi not connected...");
            lastReconnectAttempt = currentTime;
            wifiControl->reconnect();
        }
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
