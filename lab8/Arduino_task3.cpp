#include <DHT.h>

/* ----------- PIN CONFIG ----------- */

// 74HC595 pins
const int PIN_DATA = 8;   // DS
const int PIN_CLK  = 9;   // SH_CP
const int PIN_LATCH = 10; // ST_CP

// Digit control pins (common anode/cathode)
const int DIG1 = 3;
const int DIG2 = 4;
const int DIG3 = 5;
const int DIG4 = 6;

// RGB LED pins
const int RED_PIN   = A1;
const int GREEN_PIN = A2;
const int BLUE_PIN  = A3;

// DHT11
const int DHT_PIN = A4;
DHT dht(DHT_PIN, DHT11);

/* ----------- SEGMENT TABLE ----------- */
// 0b(dp)(g)(f)(e)(d)(c)(b)(a)
const uint8_t segCode[10] = {
  0b00111111, //0
  0b00000110, //1
  0b01011011, //2
  0b01001111, //3
  0b01100110, //4
  0b01101101, //5
  0b01111101, //6
  0b00000111, //7
  0b01111111, //8
  0b01101111  //9
};

/* ----------- 74HC595 OUTPUT ----------- */

void shiftOut595(uint8_t data) {
  digitalWrite(PIN_LATCH, LOW);
  shiftOut(PIN_DATA, PIN_CLK, MSBFIRST, data);
  digitalWrite(PIN_LATCH, HIGH);
}

/* ----------- SHOW DIGIT ----------- */

void showDigit(int digit, uint8_t code) {
  // turn all OFF
  digitalWrite(DIG1, LOW);
  digitalWrite(DIG2, LOW);
  digitalWrite(DIG3, LOW);
  digitalWrite(DIG4, LOW);

  shiftOut595(code);

  switch (digit) {
    case 1: digitalWrite(DIG1, HIGH); break;
    case 2: digitalWrite(DIG2, HIGH); break;
    case 3: digitalWrite(DIG3, HIGH); break;
    case 4: digitalWrite(DIG4, HIGH); break;
  }

  delay(2);
}

/* ----------- RGB CONTROL ----------- */

void rgb_set(float h) {

  if (h < 40) {          // недостатня вологість
    digitalWrite(RED_PIN, LOW);
    digitalWrite(GREEN_PIN, LOW);
    digitalWrite(BLUE_PIN, HIGH);
  }
  else if (h <= 60) {    // норма
    digitalWrite(RED_PIN, LOW);
    digitalWrite(GREEN_PIN, HIGH);
    digitalWrite(BLUE_PIN, LOW);
  }
  else {                 // висока вологість
    digitalWrite(RED_PIN, HIGH);
    digitalWrite(GREEN_PIN, LOW);
    digitalWrite(BLUE_PIN, LOW);
  }
}

/* ----------- SETUP ----------- */

void setup() {
  dht.begin();

  pinMode(PIN_DATA, OUTPUT);
  pinMode(PIN_CLK, OUTPUT);
  pinMode(PIN_LATCH, OUTPUT);

  pinMode(DIG1, OUTPUT);
  pinMode(DIG2, OUTPUT);
  pinMode(DIG3, OUTPUT);
  pinMode(DIG4, OUTPUT);

  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
}

/* ----------- LOOP ----------- */

unsigned long lastRead = 0;
float temp = 0, hum = 0;

uint8_t t1, t2, h1, h2;

void loop() {

  /* --- Read every 100 ms --- */
  if (millis() - lastRead >= 100) {
    lastRead = millis();

    float h = dht.readHumidity();
    float t = dht.readTemperature();

    if (!isnan(h) && !isnan(t)) {
      hum = h;
      temp = t;

      int T = constrain((int)temp, 0, 99);
      int H = constrain((int)hum, 0, 99);

      t1 = T / 10;
      t2 = T % 10;
      h1 = H / 10;
      h2 = H % 10;
    }

    rgb_set(hum);
  }

  /* --- Multiplex display --- */
  showDigit(1, segCode[t1]);
  showDigit(2, segCode[t2] | 0b10000000);  // точка після температури
  showDigit(3, segCode[h1]);
  showDigit(4, segCode[h2]);
}
