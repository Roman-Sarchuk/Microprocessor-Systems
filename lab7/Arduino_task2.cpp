#include <Arduino.h>

const  int segPins[8] = {2,3,4,5,6,7,8,13};
const  int digPins[3] = {9,10,11};

const  int RED_PIN = A1;
const  int GREEN_PIN = A2;
const  int BLUE_PIN = A3;

const  int LM35_PIN = A0;

const  int digitCode[10] = {
  0x3F, // 0
  0x06, // 1
  0x5B, // 2
  0x4F, // 3
  0x66, // 4
  0x6D, // 5
  0x7D, // 6
  0x07, // 7
  0x7F, // 8
  0x6F  // 9
};

void showDigitRaw( int code) {
  for ( int i=0;i<8;i++) {
    digitalWrite(segPins[i], (code & (1<<i)) ? HIGH : LOW);
  }
}

void displayOne( int pos, int8_t value, bool dp) {
  for ( int i=0;i<3;i++) digitalWrite(digPins[i], LOW);

  if (value >= 0 && value <= 9) {
     int code = digitCode[( int)value];
    if (dp) code |= 0x80;
    showDigitRaw(code);
    digitalWrite(digPins[pos], HIGH);
  } else {
    showDigitRaw(0x00);
    digitalWrite(digPins[pos], LOW);
  }
}

void setRGB(float t) {
  if (t >= 33.0) {
    digitalWrite(RED_PIN, HIGH);
    digitalWrite(GREEN_PIN, LOW);
    digitalWrite(BLUE_PIN, LOW);
  } else if (t >= 25.0) {
    digitalWrite(RED_PIN, LOW);
    digitalWrite(GREEN_PIN, HIGH);
    digitalWrite(BLUE_PIN, LOW);
  } else if (t > 18.0) {
    digitalWrite(RED_PIN, LOW);
    digitalWrite(GREEN_PIN, LOW);
    digitalWrite(BLUE_PIN, HIGH);
  } else {
    digitalWrite(RED_PIN, LOW);
    digitalWrite(GREEN_PIN, LOW);
    digitalWrite(BLUE_PIN, LOW);
  }
}

void setup() {
  // pins
  for ( int i=0;i<8;i++) pinMode(segPins[i], OUTPUT);
  for ( int i=0;i<3;i++) pinMode(digPins[i], OUTPUT);

  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);

  pinMode(LM35_PIN, INPUT);

  // ensure off
  for ( int i=0;i<8;i++) digitalWrite(segPins[i], LOW);
  for ( int i=0;i<3;i++) digitalWrite(digPins[i], LOW);
  digitalWrite(RED_PIN, LOW);
  digitalWrite(GREEN_PIN, LOW);
  digitalWrite(BLUE_PIN, LOW);
}

void loop() {
  int raw = analogRead(LM35_PIN);
  float voltage = raw * 5.0 / 1023.0;
  float tempC = voltage * 100.0;
  int temp10 = (int)(tempC * 10.0 + 0.5);

  int d2 = (temp10 / 100) % 10;
  int d1 = (temp10 / 10) % 10;
  int d0 = temp10 % 10;

  bool leftBlank = (temp10 < 100);

  setRGB(tempC);

  unsigned long t0 = millis();
  while (millis() - t0 < 200) {
    if (leftBlank) {
      displayOne(0, -1, false);
      delay(3);
    } else {
      displayOne(0, d2, false);
      delay(3);
    }
    displayOne(1, d1, true);
    delay(3);
    displayOne(2, d0, false);
    delay(3);
  }

  delay(50);
}