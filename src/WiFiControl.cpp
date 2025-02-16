#include <esp_wps.h>
#include "WiFiControl.h"

#define ESP_WPS_MODE WPS_TYPE_PBC

WiFiControl* WiFiControl::instance = nullptr;

WiFiControl::WiFiControl(Preferences* prefs):
    preferences(prefs) {
    instance = this;
}

void WiFiControl::setupWiFi() {
    Serial.println("WiFi Control: Setting up WiFi...");

    WiFi.mode(WIFI_MODE_STA);
    WiFi.onEvent(staticHandleWiFiEvent);

    Serial.println("WiFi Control: Loading WiFi credentials.");
    String ssid = preferences->getString("ssid", "");;
    String password = preferences->getString("password", "");

    if (!ssid.isEmpty() && !password.isEmpty()) {
        Serial.println("WiFi Control: SSID and password found. Connecting to WiFi.");
        WiFi.begin(ssid.c_str(), password.c_str());
    } else {
        Serial.println("WiFi Control: SSID and password not found. Starting WPS.");
        wpsStart();
    }
}

bool WiFiControl::isConnected() {
    return WiFi.status() == WL_CONNECTED;
}

void WiFiControl::reconnect() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi Control: Reconnecting to WiFi.");
        WiFi.reconnect();
    }
}

void WiFiControl::clearCredentials() {
    Serial.println("WiFi Control: Clearing WiFi credentials.");
    preferences->remove("ssid");
    preferences->remove("password");
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
        Serial.printf("WiFi Control: WPS Enable Failed: 0x%x: %s\n", err, esp_err_to_name(err));
        return;
    }

    err = esp_wifi_wps_start(0);
    if (err != ESP_OK) {
        Serial.printf("WiFi Control: WPS Start Failed: 0x%x: %s\n", err, esp_err_to_name(err));
    }
}

void WiFiControl::wpsStop() {
    esp_err_t err = esp_wifi_wps_disable();
    if (err != ESP_OK) {
        Serial.printf("WiFi Control: WPS Disable Failed: 0x%x: %s\n", err, esp_err_to_name(err));
    }
}

void WiFiControl::staticHandleWiFiEvent(WiFiEvent_t event, arduino_event_info_t info) {
    if (instance) {
        instance->handleWiFiEvent(event, info);
    }
}

void WiFiControl::handleWiFiEvent(WiFiEvent_t event, arduino_event_info_t info) {
    switch (event) {
        case ARDUINO_EVENT_WIFI_STA_START:
            Serial.println("WiFi Control: Station Mode Started");
            break;
        case ARDUINO_EVENT_WIFI_STA_GOT_IP:
            Serial.println("WiFi Control: Connected to: " + String(WiFi.SSID()));
            Serial.print("WiFi Control: Got IP: ");
            Serial.println(WiFi.localIP());
            Serial.println("WiFi Control: Storing credentials");

            instance->preferences->putString("ssid", WiFi.SSID());
            instance->preferences->putString("password", WiFi.psk());
            break;
        case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
            Serial.println("WiFi Control: Disconnected from station, attempting reconnection");
            break;
        case ARDUINO_EVENT_WPS_ER_SUCCESS:
            Serial.println("WiFi Control: WPS Successful, stopping WPS");
            wpsStop();
            break;
        case ARDUINO_EVENT_WPS_ER_FAILED:
            Serial.println("WiFi Control: WPS Failed, retrying");
            wpsStart();
            break;
        case ARDUINO_EVENT_WPS_ER_TIMEOUT:
            Serial.println("WiFi Control: WPS Timedout, retrying");
            wpsStart();
            break;
        default:
            Serial.println("WiFi Control: Unhandled event from WiFi");
            break;
    }
}

void WiFiControl::displayWiFiSignalStrength() {
    static unsigned long lastPrintTime = 0;
    unsigned long currentTime = millis();

    if (currentTime - lastPrintTime >= 2000) {
        lastPrintTime = currentTime;
        int rssi = WiFi.RSSI();
        Serial.printf("WiFi Control: RSSI: %d dBm\n", rssi);
    }
}
