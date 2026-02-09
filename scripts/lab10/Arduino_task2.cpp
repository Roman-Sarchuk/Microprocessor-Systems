#include "LedControl.h"

LedControl lc = LedControl(12, 11, 10, 1);
unsigned long delayTime = 150;  // затримка між кроками

void setup() {
  lc.shutdown(0, false);
  lc.setIntensity(0, 8);
  lc.clearDisplay(0);
}

void setPixel(int row, int col) {
  lc.clearDisplay(0);
  lc.setLed(0, row, col, true);
}

void loop() {
  for (int col = 0; col < 8; col++) {
    setPixel(0, col);
    delay(delayTime);
  }

  for (int row = 1; row < 8; row++) {
    setPixel(row, 7);
    delay(delayTime);
  }

  for (int col = 6; col >= 0; col--) {
    setPixel(7, col);
    delay(delayTime);
  }

  for (int row = 6; row > 0; row--) {
    setPixel(row, 0);
    delay(delayTime);
  }
}
