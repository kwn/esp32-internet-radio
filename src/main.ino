#include <Audio.h>
#include <Encoder.h>

#include "VolumeControl.h"
#include "StationControl.h"
#include "WiFiControl.h"
#include "LedControl.h"

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

Audio audio;
Preferences preferences;
Encoder *encoder1;
Encoder *encoder2;

VolumeControl *volumeControl;
StationControl *stationControl;
WiFiControl *wifiControl;
LedControl *ledControl;

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
  pinMode(PIN_ENCODER1_SW, INPUT);
  pinMode(PIN_ENCODER2_DT, INPUT);
  pinMode(PIN_ENCODER2_CLK, INPUT);
  pinMode(PIN_ENCODER2_SW, INPUT);

  encoder1 = new Encoder(PIN_ENCODER1_CLK, PIN_ENCODER1_DT);
  encoder2 = new Encoder(PIN_ENCODER2_CLK, PIN_ENCODER2_DT);
  volumeControl = new VolumeControl(encoder2, &audio, PIN_ENCODER2_SW);
  stationControl = new StationControl(encoder1, &audio);
  wifiControl = new WiFiControl(preferences);
  ledControl = new LedControl(PIN_LED_RED, PIN_LED_GREEN, PIN_LED_BLUE);

  ledControl->setColour(COLOUR_RED);

  Serial.println("Setting up audio...");

  audio.setPinout(PIN_I2S_BCLK, PIN_I2S_LRC, PIN_I2S_DOUT);
  audio.forceMono(true);

  wifiControl->setupWiFi();
}

void loop() {
  volumeControl->handleVolume();
  volumeControl->handleMute();
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
  String sinfo=String(info);

  // Serial.println("Station: " + sinfo.substring(0,16));

  //   if(sinfo.startsWith("Bitrate=")) {
  //     Serial.println("Bitrate " + sinfo.substring(8).toInt());
  //   } else if(sinfo.startsWith("StreamTitle=")) {
  //     Serial.println("Title: " + sinfo.substring(12,44));
  //   } else if(sinfo.startsWith("Channels=")) {
  //     Serial.println("Channels" + sinfo.substring(8).toInt());
    // } else if(sinfo.startsWith("SampleRate=")) {
    //   env["stream_samplerate"] = sinfo.substring(11).toInt();
    // } else if(sinfo.startsWith("BitsPerSample=")) {
    //   env["stream_bitspersample"] = sinfo.substring(14).toInt();
    // } else if(sinfo.startsWith("icy-genre:")) { // Get the stream genre, if present
    //   env["stream_genre"] = sinfo.substring(10,24);
    // } else {
      Serial.println(sinfo);
    // }
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
