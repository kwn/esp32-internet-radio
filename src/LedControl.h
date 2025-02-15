#ifndef LED_CONTROL_H
#define LED_CONTROL_H

#include <Arduino.h>

#define MAX_BLINKS 10

struct Colour {
    int red;
    int green;
    int blue;

    bool operator==(const Colour &other) const {
        return red == other.red && green == other.green && blue == other.blue;
    }

    bool operator!=(const Colour &other) const {
        return !(*this == other);
    }
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
const Colour COLOUT_OFF = {0, 0, 0};

class LedControl {
private:
    int pinRed, pinGreen, pinBlue;
    Colour currentColour;

public:
    LedControl(int redPin, int greenPin, int bluePin);
    void setColour(const Colour &colour);
    void turnOff();
    void blink(const Colour &colour, int times);
};


#endif // LED_CONTROL_H
