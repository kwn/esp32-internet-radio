#ifndef DEBOUNCER_H
#define DEBOUNCER_H

#include <Arduino.h>

class Debouncer {
public:
    Debouncer(unsigned long interval_ms) : interval(interval_ms), previousMillis(0) {}

    bool hasElapsed() {
        unsigned long currentMillis = millis();
        if (currentMillis - previousMillis >= interval) {
            previousMillis = currentMillis;
            return true;
        }
        return false;
    }

private:
    unsigned long interval;
    unsigned long previousMillis;
};

#endif // DEBOUNCER_H 