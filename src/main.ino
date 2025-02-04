#include <WiFi.h>
#include <esp_wps.h>
#include <Preferences.h>
#include <Audio.h>
#include <Encoder.h>

#include "VolumeControl.h"
#include "StationControl.h"

#define PIN_LED_RED 21
#define PIN_LED_GREEN 22
#define PIN_LED_BLUE 23
#define PIN_I2S_DOUT 26
#define PIN_I2S_BCLK 27
#define PIN_I2S_LRC 25
#define PIN_ENCODER_CLK 5
#define PIN_ENCODER_DT 18
#define PIN_ENCODER_SW 19

#define PIN_ENCODER2_CLK 34
#define PIN_ENCODER2_DT 35
#define PIN_ENCODER2_SW 32

#define ESP_WPS_MODE WPS_TYPE_PBC
#define MAX_BLINKS 10

struct Colour {
  int red;
  int green;
  int blue;
};

struct Blink {
  Colour colour;
  int times;
  const char* message;
};

const Colour COLOUR_RED = {255, 0, 0};
const Colour COLOUR_GREEN = {0, 255, 0};
const Colour COLOUR_BLUE = {0, 0, 255};
const Colour COLOUR_YELLOW = {255, 255, 0};
const Colour COLOUR_MAGENTA = {255, 0, 255};
const Colour COLOUR_CYAN = {0, 255, 255};
const Colour COLOUR_WHITE = {255, 255, 255};

Audio audio;
Preferences preferences;
Encoder *encoder;
Encoder *encoder2;
Blink blinks[MAX_BLINKS];
VolumeControl *volumeControl;
StationControl *stationControl;

int blinkCount = 0;
int currentRed = 0;
int currentGreen = 0;
int currentBlue = 0;
long currentEncoder = 0;
int currentStation = 0;

void setup() {
  Serial.begin(9600);

  Serial.println("Starting setup...");

  pinMode(PIN_LED_RED, OUTPUT);
  pinMode(PIN_LED_GREEN, OUTPUT);
  pinMode(PIN_LED_BLUE, OUTPUT);
  pinMode(PIN_I2S_BCLK, OUTPUT);
  pinMode(PIN_I2S_DOUT, OUTPUT);
  pinMode(PIN_I2S_LRC, OUTPUT);
  pinMode(PIN_ENCODER_DT, INPUT);
  pinMode(PIN_ENCODER_CLK, INPUT);
  pinMode(PIN_ENCODER_SW, INPUT_PULLUP);
  pinMode(PIN_ENCODER2_DT, INPUT);
  pinMode(PIN_ENCODER2_CLK, INPUT);
  pinMode(PIN_ENCODER2_SW, INPUT);

  changeColour(COLOUR_BLUE);

  encoder = new Encoder(PIN_ENCODER_CLK, PIN_ENCODER_DT);
  encoder2 = new Encoder(PIN_ENCODER2_CLK, PIN_ENCODER2_DT);

  Serial.println("Setting up audio...");

  audio.setPinout(PIN_I2S_BCLK, PIN_I2S_LRC, PIN_I2S_DOUT);
  audio.setBalance(0);

  volumeControl = new VolumeControl(encoder2, &audio);
  stationControl = new StationControl(encoder, &audio);

  Serial.println("Initiating preferences...");

  preferences.begin("wifi", false);
  String ssid = preferences.getString("ssid", "");
  String password = preferences.getString("password", "");

  Serial.println("Setting up WiFi...");

  WiFi.onEvent(WiFiEvent);
  WiFi.mode(WIFI_MODE_STA);

  if (!ssid.isEmpty() && !password.isEmpty()) {
    Serial.println("SSID and password found. Connecting to WiFi.");
    WiFi.begin(ssid.c_str(), password.c_str());
  } else {
    Serial.println("SSID and password not found. Starting WPS.");
    wpsStart();
  }
}

void loop() {
  // processNextBlink();

  // long newPosition = (int)(encoder->read() / 4);

  // if (newPosition != currentEncoder) {
  //   Serial.println("New position: " + String(newPosition));
  //   currentEncoder = newPosition;
  // }

  volumeControl->handleVolume();
  stationControl->handleStationChange();

  // handleClick();

  if (WiFi.status() == WL_CONNECTED) {
    if (audio.isRunning()) {
      audio.loop();
    } else {
      Serial.println("Audio stopped or failed, attempting to reconnect...");
      delay(1000);
      stationControl->reconnect();
    }
  } else {
    Serial.println("Wifi not connected...");

    changeColour(COLOUR_RED);
    delay(1000);
  }
}

void handleClick() {
  Serial.println("PIN STATE " + String(analogRead(PIN_ENCODER2_SW)));
  if (digitalRead(PIN_ENCODER2_SW) == LOW) {
    // Serial.println("Mute click");
    //currentVolume = 0;
    //audio.setVolume(currentVolume);
    // if(currentStation >= numStations - 1) {
    //   currentStation = 0;
    // } else {
    //   currentStation++;
    // }

    // delay(200);
    // audio.connecttohost(RADIO_STATIONS[currentStation]);
    // Serial.println("Set station to " + String(currentStation));
    }
}

void addBlink(Blink blink) {
  if (blinkCount < MAX_BLINKS) {
    blinks[blinkCount++] = blink;
  }
}

void processNextBlink() {
  if (blinkCount > 0) {
    Blink currentBlink = blinks[0];

    for (int i = 0; i < blinkCount - 1; i++) {
      blinks[i] = blinks[i + 1];
    }
    blinkCount--;

    blink(currentBlink);
  }
}

void setColor(int red, int green, int blue) {
  analogWrite(PIN_LED_RED, red);
  analogWrite(PIN_LED_GREEN, green);
  analogWrite(PIN_LED_BLUE, blue);

  currentRed = red;
  currentGreen = green;
  currentBlue = blue;
}

void turnLedOff() {
  analogWrite(PIN_LED_RED, 0);
  analogWrite(PIN_LED_GREEN, 0);
  analogWrite(PIN_LED_BLUE, 0);
}

void blink(Blink blink) {
  int prevRed = currentRed;
  int prevGreen = currentGreen;
  int prevBlue = currentBlue;

  Serial.println(blink.message);

  for (int i = 0; i < blink.times; i++) {
    Serial.printf("Set color: %d %d %d\n", blink.colour.red, blink.colour.green, blink.colour.blue);
    setColor(blink.colour.red, blink.colour.green, blink.colour.blue);
    delay(200);

    Serial.println("Turn led off");
    turnLedOff();
    delay(200);
  }

  delay(2000);

  setColor(prevRed, prevGreen, prevBlue);
}

void wpsStart() {
  esp_wps_config_t config;
  memset(&config, 0, sizeof(esp_wps_config_t));

  config.wps_type = ESP_WPS_MODE;
  strcpy(config.factory_info.manufacturer, "ESPRESSIF");
  strcpy(config.factory_info.model_number, CONFIG_IDF_TARGET);
  strcpy(config.factory_info.model_name, "ESPRESSIF IOT");
  strcpy(config.factory_info.device_name, "ESP DEVICE");

  esp_err_t err = esp_wifi_wps_enable(&config);
  if (err != ESP_OK) {
    Serial.printf("WPS Enable Failed: 0x%x: %s\n", err, esp_err_to_name(err));
    return;
  }

  err = esp_wifi_wps_start(0);
  if (err != ESP_OK) {
    Serial.printf("WPS Start Failed: 0x%x: %s\n", err, esp_err_to_name(err));
  }
}

void wpsStop() {
  esp_err_t err = esp_wifi_wps_disable();
  if (err != ESP_OK) {
    Serial.printf("WPS Disable Failed: 0x%x: %s\n", err, esp_err_to_name(err));
  }
}

void WiFiEvent(WiFiEvent_t event, arduino_event_info_t info) {
  switch (event) {
    case ARDUINO_EVENT_WIFI_STA_START: {
      Serial.println("Station Mode Started");
      //addBlink({COLOUR_WHITE, 3, "ARDUINO_EVENT_WIFI_STA_START"});
      break;
    }
    case ARDUINO_EVENT_WIFI_STA_GOT_IP: {
      Serial.println("Connected to: " + String(WiFi.SSID()));
      Serial.print("Got IP: ");
      Serial.println(WiFi.localIP());
      //addBlink({COLOUR_GREEN, 3, "ARDUINO_EVENT_WIFI_STA_GOT_IP"});
      break;
    }
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED: {
      Serial.println("Disconnected from station, attempting reconnection");
      WiFi.reconnect();
      //addBlink({COLOUR_YELLOW, 3, "ARDUINO_EVENT_WIFI_STA_DISCONNECTED"});
      break;
    }
    case ARDUINO_EVENT_WPS_ER_SUCCESS: {
      Serial.println("WPS Successful, stopping WPS, saving credentials and connecting to: " + String(WiFi.SSID()));

      wpsStop();

      String ssid = WiFi.SSID();
      String pass = WiFi.psk();

      preferences.putString("ssid", ssid);
      preferences.putString("password", pass);

      WiFi.begin(ssid.c_str(), pass.c_str());

      //addBlink({COLOUR_BLUE, 3, "ARDUINO_EVENT_WPS_ER_SUCCESS"});
      break;
    }
    case ARDUINO_EVENT_WPS_ER_FAILED: {
      Serial.println("WPS Failed, retrying");
      wpsStop();
      wpsStart();
      //addBlink({COLOUR_BLUE, 4, "ARDUINO_EVENT_WPS_ER_FAILED"});
      break;
    }
    case ARDUINO_EVENT_WPS_ER_TIMEOUT: {
      Serial.println("WPS Timedout, retrying");
      wpsStop();
      wpsStart();
      //addBlink({COLOUR_MAGENTA, 4, "ARDUINO_EVENT_WPS_ER_TIMEOUT"});
      break;
    }
    default: {
      Serial.println("Unhandled even from WiFi: " + event);
      break;
    }
  }
}

void clearWifiSettings() {
  preferences.remove("ssid");
  preferences.remove("password");
}

void changeColour(Colour colour) {
  setColor(colour.red, colour.green, colour.blue);
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
