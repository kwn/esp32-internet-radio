#ifndef WIFICONTROL_H
#define WIFICONTROL_H

#include <WiFi.h>
#include <esp_wps.h>
#include <Preferences.h>

class WiFiControl {
private:
    Preferences preferences;

    static void wpsStart();
    static void wpsStop();
    static void handleWiFiEvent(WiFiEvent_t event, arduino_event_info_t info);

public:
    WiFiControl(Preferences prefs);
    void setupWiFi();
    void reconnect();
    void clearCredentials();
    void saveCredentials(const String& ssid, const String& password);
    void loadCredentials(String& ssid, String& password);
};

#endif
