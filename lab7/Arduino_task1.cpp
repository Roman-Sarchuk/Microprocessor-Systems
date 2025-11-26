#include <LiquidCrystal.h>

const int RS = 12;
const int E = 11;
const int D4 = 5;
const int D5 = 4;
const int D6 = 3;
const int D7 = 2;

LiquidCrystal lcd(RS, E, D4, D5, D6, D7);

const int LM35_PIN = A0;
const int RED_PIN = 8;
const int GREEN_PIN = 9;
const int BLUE_PIN = 10;

void setup() {
  lcd.begin(16, 2);
  lcd.clear();

  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
}

void loop() {
  int reading = analogRead(LM35_PIN);
  float temperature = (reading * 5.0 * 100.0) / 1023.0;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temperature:");
  lcd.setCursor(0, 1);
  lcd.print(temperature, 1);
  lcd.print((char)223);
  lcd.print("C");

  if (temperature <= 18) {
    digitalWrite(RED_PIN, LOW);
    digitalWrite(GREEN_PIN, LOW);
    digitalWrite(BLUE_PIN, LOW);
  } else if (temperature > 18 && temperature < 25) {
    digitalWrite(RED_PIN, LOW);
    digitalWrite(GREEN_PIN, LOW);
    digitalWrite(BLUE_PIN, HIGH);
  } else if (temperature >= 25 && temperature < 33) {
    digitalWrite(RED_PIN, LOW);
    digitalWrite(GREEN_PIN, HIGH);
    digitalWrite(BLUE_PIN, LOW);
  } else if (temperature >= 33) {
    digitalWrite(RED_PIN, HIGH);
    digitalWrite(GREEN_PIN, LOW);
    digitalWrite(BLUE_PIN, LOW);
  }

  delay(1000);
}