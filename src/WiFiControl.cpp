#include "WiFiControl.h"

WiFiControl::WiFiControl(Preferences prefs) {
    preferences = prefs;

    preferences.begin("wifi", false);
}

void WiFiControl::setupWiFi() {
    Serial.println("Setting up WiFi...");

    WiFi.onEvent(handleWiFiEvent);
    WiFi.mode(WIFI_MODE_STA);

    String ssid, password;
    loadCredentials(ssid, password);

    if (!ssid.isEmpty() && !password.isEmpty()) {
        Serial.println("SSID and password found. Connecting to WiFi.");
        WiFi.begin(ssid.c_str(), password.c_str());
    } else {
        Serial.println("SSID and password not found. Starting WPS.");
        wpsStart();
    }
}

void WiFiControl::reconnect() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("Reconnecting to WiFi.");
        WiFi.reconnect();
    }
}

void WiFiControl::clearCredentials() {
    Serial.println("Clearing WiFi credentials.");
    preferences.remove("ssid");
    preferences.remove("password");
}

void WiFiControl::saveCredentials(const String& ssid, const String& password) {
    Serial.println("Saving WiFi credentials.");
    preferences.putString("ssid", ssid);
    preferences.putString("password", password);
}

void WiFiControl::loadCredentials(String& ssid, String& password) {
    Serial.println("Loading WiFi credentials.");
    ssid = preferences.getString("ssid", "");
    password = preferences.getString("password", "");
}

void WiFiControl::wpsStart() {
    esp_wps_config_t config;
    memset(&config, 0, sizeof(esp_wps_config_t));

    config.wps_type = WPS_TYPE_PBC;
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

void WiFiControl::wpsStop() {
    esp_err_t err = esp_wifi_wps_disable();
    if (err != ESP_OK) {
        Serial.printf("WPS Disable Failed: 0x%x: %s\n", err, esp_err_to_name(err));
    }
}

void WiFiControl::handleWiFiEvent(WiFiEvent_t event, arduino_event_info_t info) {
    switch (event) {
        case ARDUINO_EVENT_WIFI_STA_START:
            Serial.println("Station Mode Started");
            break;
        case ARDUINO_EVENT_WIFI_STA_GOT_IP:
            Serial.println("Connected to: " + String(WiFi.SSID()));
            Serial.print("Got IP: ");
            Serial.println(WiFi.localIP());
            break;
        case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
            Serial.println("Disconnected from station, attempting reconnection");
            break;
        case ARDUINO_EVENT_WPS_ER_SUCCESS:
            Serial.println("WPS Successful, stopping WPS, saving credentials and connecting to: " + String(WiFi.SSID()));
            wpsStop();
            break;
        case ARDUINO_EVENT_WPS_ER_FAILED:
            Serial.println("WPS Failed, retrying");
            wpsStart();
            break;
        case ARDUINO_EVENT_WPS_ER_TIMEOUT:
            Serial.println("WPS Timedout, retrying");
            wpsStart();
            break;
        default:
            Serial.println("Unhandled event from WiFi");
            break;
    }
}
