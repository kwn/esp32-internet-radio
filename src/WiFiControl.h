#ifndef WIFICONTROL_H
#define WIFICONTROL_H

#include <WiFi.h>
#include <esp_wps.h>
#include <Preferences.h>

class WiFiControl {
private:
    Preferences *preferences;
    static WiFiControl* instance;

    void wpsStart();
    void wpsStop();
    void saveCredentials(const String& ssid, const String& password);
    void loadCredentials(String& ssid, String& password);
    void handleWiFiEvent(WiFiEvent_t event, arduino_event_info_t info);
    static void staticHandleWiFiEvent(WiFiEvent_t event, arduino_event_info_t info);

public:
    WiFiControl(Preferences* prefs);
    void setupWiFi();
    bool isConnected();
    void reconnect();
    void clearCredentials();
    void displayWiFiSignalStrength();
};

#endif
