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
#include "Debouncer.h"

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

// Global volume attenuation factor (0.0 to 1.0) for manual software gain
float volumeAttenuation = 0.1; // Default to 10% volume

VolumeControl *volumeControl;
StationControl *stationControl;
WiFiControl *wifiControl;
LedControl *ledControl;
ToneControl *toneControl;
StatusControl *statusControl;

Debouncer wifiReconnectDebouncer(5000);
Debouncer streamReconnectDebouncer(5000);

void audio_process_i2s(uint32_t* sample, bool* continueI2S) {
    // Unpack the 32-bit sample into two 16-bit channels
    int16_t sample_l = (*sample >> 16) & 0xFFFF;
    int16_t sample_r = *sample & 0xFFFF;

    // Apply our custom volume attenuation to each channel
    sample_l = (int16_t)((float)sample_l * volumeAttenuation);
    sample_r = (int16_t)((float)sample_r * volumeAttenuation);

    // Repack the attenuated channels back into a 32-bit sample
    *sample = (uint32_t)sample_l << 16 | (uint16_t)sample_r;

    *continueI2S = true;
}

void ledTask(void *pvParameters) {
    for (;;) {
        ledControl->update();
        vTaskDelay(17 / portTICK_PERIOD_MS);
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

    audio.setPinout(PIN_I2S_BCLK, PIN_I2S_LRC, PIN_I2S_DOUT);
    audio.setVolume(11); // Set a safe, moderate library volume. We control the rest.
    audio.forceMono(true);

    FastLED.addLeds<LED_TYPE, PIN_LED_DATA, LED_COLOR_ORDER>(leds, LED_NUMBER);
    FastLED.setBrightness(LED_BRIGHTNESS);

    statusControl = new StatusControl();
    stationControl = new StationControl(&audio, &preferences, statusControl, PIN_ENCODER1_CLK, PIN_ENCODER1_DT, PIN_ENCODER1_SW);
    ledControl = new LedControl(leds, LED_NUMBER, statusControl, stationControl);
    toneControl = new ToneControl(&audio, &preferences, ledControl, PIN_ENCODER2_CLK, PIN_ENCODER2_DT, PIN_ENCODER2_SW);
    volumeControl = new VolumeControl(&audio, &preferences, statusControl, ledControl, PIN_ENCODER3_CLK, PIN_ENCODER3_DT, PIN_ENCODER3_SW);
    wifiControl = new WiFiControl(&preferences);

    xTaskCreatePinnedToCore(ledTask, "LED Update Task", 2048, NULL, 1, NULL, 1);

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

        if (audio.isRunning()) {
            statusControl->setPrimaryState(STATE_PLAYING);
            audio.loop();
        } else {
            statusControl->setPrimaryState(STATE_STREAM_BUFFERING);
            if (streamReconnectDebouncer.hasElapsed()) {
                Serial.println("Main: Audio stopped or failed, attempting to reconnect...");
                stationControl->reconnect();
            }
        }
    } else {
        statusControl->setPrimaryState(STATE_WIFI_CONNECTING);
        if (wifiReconnectDebouncer.hasElapsed()) {
            Serial.println("Main: Wifi not connected...");
            wifiControl->reconnect();
        }
    }
}

void audio_info(const char *info) {
    Serial.println("Main: Audio info: " + String(info));
}

void audio_id3data(const char *info) {
    Serial.println("Main: ID3 info: " + String(info));
}

void audio_showstreamtitle(const char *info) {
    Serial.println("Main: Stream info: " + String(info));
}
