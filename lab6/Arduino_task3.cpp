#include <LiquidCrystal.h>

const int RS = 8;
const int E = 9;
const int D4 = 4;
const int D5 = 5;
const int D6 = 6;
const int D7 = 7;
const int BtnPin = 2;

LiquidCrystal lcd(RS, E, D4, D5, D6, D7);
bool lastButtonState = HIGH;
bool buttonState = HIGH;


void printRandNum() {
  lcd.clear();
  lcd.print(random());
}


void setup() {
  lcd.begin(16, 2);

  pinMode(BtnPin, INPUT_PULLUP);

  printRandNum();
}

void loop() {
  buttonState = digitalRead(BtnPin);

  if (lastButtonState == HIGH && buttonState == LOW) {
    printRandNum();
  }

  lastButtonState = buttonState;
}
