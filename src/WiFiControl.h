#ifndef WIFICONTROL_H
#define WIFICONTROL_H

#include <WiFi.h>
#include <esp_wps.h>
#include <Preferences.h>

class WiFiControl {
public:
    WiFiControl();
    void setupWiFi();
    void reconnect();
    void clearCredentials();

private:
    Preferences preferences;
    void wpsStart();
    void wpsStop();
    static void WiFiEvent(WiFiEvent_t event, arduino_event_info_t info);
};

#endif
