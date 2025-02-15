#include "LedControl.h"

Blink blinks[MAX_BLINKS];

LedControl::LedControl(int redPin, int greenPin, int bluePin)
  : pinRed(redPin), pinGreen(greenPin), pinBlue(bluePin) {}

void LedControl::setColour(const Colour &colour) {
    if (currentColour != colour) {
        analogWrite(pinRed, colour.red);
        analogWrite(pinGreen, colour.green);
        analogWrite(pinBlue, colour.blue);

        currentColour = colour;
    }
}

void LedControl::turnOff() {
    setColour(COLOUT_OFF);
}

void LedControl::blink(const Colour &colour, int times) {
    for (int i = 0; i < times; i++) {
        setColour(colour);
        delay(200);
        turnOff();
        delay(200);
    }
    delay(2000);
    setColour(currentColour);
}

// void addBlink(Blink blink) {
//   if (blinkCount < MAX_BLINKS) {
//     blinks[blinkCount++] = blink;
//   }
// }

// void processNextBlink() {
//   if (blinkCount > 0) {
//     Blink currentBlink = blinks[0];

//     for (int i = 0; i < blinkCount - 1; i++) {
//       blinks[i] = blinks[i + 1];
//     }
//     blinkCount--;

//     blink(currentBlink);
//   }
// }

