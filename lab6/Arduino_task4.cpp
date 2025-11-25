#include <LiquidCrystal.h>

const int RS = 8;
const int E = 9;
const int D4 = 4;
const int D5 = 5;
const int D6 = 6;
const int D7 = 7;
const int BtnPlayPin = 2;
const int BtnStopPin = 3;

LiquidCrystal lcd(RS, E, D4, D5, D6, D7);
bool btnPlayPressed = 0;
bool btnStopPressed = 0;


void setup() {
  lcd.begin(16, 2);

  pinMode(BtnPlayPin, INPUT_PULLUP);
  pinMode(BtnStopPin, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(BtnPlayPin), handleBtnPlay, FALLING);
  attachInterrupt(digitalPinToInterrupt(BtnStopPin), handleBtnStop, FALLING);

  lcd.print("LOAD");
}

void handleBtnPlay() {
  btnPlayPressed = 1;
}

void handleBtnStop() {
  btnStopPressed = 1;
}

void loop() {
  if (btnPlayPressed) {
    lcd.clear();
    lcd.print("PLAY");
    btnPlayPressed = 0;
  }

  if (btnStopPressed) {
    lcd.clear();
    lcd.print("STOP");
    btnStopPressed = 0;
  }
}
