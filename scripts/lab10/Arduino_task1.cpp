#include "LedControl.h"
#include "binary.h"

// MAX7219 connection:
// DIN → D12
// CLK → D11
// CS  → D10
LedControl lc = LedControl(12, 11, 10, 1);

unsigned long delayTime = 150; 

void setup() {
  lc.shutdown(0, false);
  lc.setIntensity(0, 8);
  lc.clearDisplay(0);
}


void scanColumnsRightToLeft() {
  for (int col = 7; col >= 0; col--) {
    lc.clearDisplay(0);


    lc.setColumn(0, col, B11111111);

    delay(delayTime);
  }
}


void scanRowsTopToBottom() {
  for (int row = 0; row < 8; row++) {
    lc.clearDisplay(0);

    lc.setRow(0, row, B11111111);

    delay(delayTime);
  }
}

void loop() {
  scanColumnsRightToLeft();  
  scanRowsTopToBottom();      
}
