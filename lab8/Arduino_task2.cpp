#include <DHT.h>

/* ---------------- PIN CONFIG ---------------- */

const int SEG_A = 2;
const int SEG_B = 3;
const int SEG_C = 4;
const int SEG_D = 5;
const int SEG_E = 6;
const int SEG_F = 7;
const int SEG_G = 8;
const int SEG_DP = 13;

const int DIG1 = 9;   // temperature tens
const int DIG2 = 10;  // temperature ones (with dot)
const int DIG3 = 11;  // humidity tens
const int DIG4 = 12;  // humidity ones

const int RED_PIN   = A1;
const int GREEN_PIN = A2;
const int BLUE_PIN  = A3;

const int DHT_PIN = A4;
const int DHT_TYPE = DHT11;

DHT dht(DHT_PIN, DHT_TYPE);

/* ---------------- DIGIT CODES ---------------- */

const uint8_t digitCode[10] = {
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

/* ---------------- FUNCTIONS ---------------- */

// Sets segment pattern on pins
void setSegments(uint8_t code, bool dot) {
  digitalWrite(SEG_A, code & 0x01);
  digitalWrite(SEG_B, code & 0x02);
  digitalWrite(SEG_C, code & 0x04);
  digitalWrite(SEG_D, code & 0x08);
  digitalWrite(SEG_E, code & 0x10);
  digitalWrite(SEG_F, code & 0x20);
  digitalWrite(SEG_G, code & 0x40);
  digitalWrite(SEG_DP, dot);
}

void showDigit(uint8_t digit, uint8_t code, bool dot) {
  digitalWrite(DIG1, LOW);
  digitalWrite(DIG2, LOW);
  digitalWrite(DIG3, LOW);
  digitalWrite(DIG4, LOW);

  setSegments(code, dot);

  switch(digit) {
    case 1: digitalWrite(DIG1, HIGH); break;
    case 2: digitalWrite(DIG2, HIGH); break;
    case 3: digitalWrite(DIG3, HIGH); break;
    case 4: digitalWrite(DIG4, HIGH); break;
  }

  delay(2);
}

void displayAll(uint8_t t1, uint8_t t2, uint8_t h1, uint8_t h2) {
  showDigit(1, digitCode[t1], false);
  showDigit(2, digitCode[t2], true);   // dot after t2
  showDigit(3, digitCode[h1], false);
  showDigit(4, digitCode[h2], false);
}

void rgb_set(float h) {
  if (h < 40) {
    digitalWrite(RED_PIN, LOW);
    digitalWrite(GREEN_PIN, LOW);
    digitalWrite(BLUE_PIN, HIGH);
  }
  else if (h <= 60) {
    digitalWrite(RED_PIN, LOW);
    digitalWrite(GREEN_PIN, HIGH);
    digitalWrite(BLUE_PIN, LOW);
  }
  else {
    digitalWrite(RED_PIN, HIGH);
    digitalWrite(GREEN_PIN, LOW);
    digitalWrite(BLUE_PIN, LOW);
  }
}

/* ---------------- SETUP ---------------- */

void setup() {
  dht.begin();

  pinMode(SEG_A, OUTPUT);
  pinMode(SEG_B, OUTPUT);
  pinMode(SEG_C, OUTPUT);
  pinMode(SEG_D, OUTPUT);
  pinMode(SEG_E, OUTPUT);
  pinMode(SEG_F, OUTPUT);
  pinMode(SEG_G, OUTPUT);
  pinMode(SEG_DP, OUTPUT);

  pinMode(DIG1, OUTPUT);
  pinMode(DIG2, OUTPUT);
  pinMode(DIG3, OUTPUT);
  pinMode(DIG4, OUTPUT);

  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);

  digitalWrite(SEG_DP, LOW);
}

/* ---------------- LOOP ---------------- */

unsigned long lastRead = 0;
float temperature = 0, humidity = 0;

uint8_t t1=0, t2=0, h1=0, h2=0;

void loop() {

  // Update sensor every 100 ms (similar to AVR counter mechanism)
  if (millis() - lastRead > 100) {
    lastRead = millis();

    float h = dht.readHumidity();
    float t = dht.readTemperature();

    if (!isnan(h) && !isnan(t)) {
      humidity = h;
      temperature = t;

      int T = (int)temperature;
      int H = (int)humidity;

      if (T < 0) T = 0;
      if (T > 99) T = 99;

      if (H < 0) H = 0;
      if (H > 99) H = 99;

      t1 = T / 10;
      t2 = T % 10;
      h1 = H / 10;
      h2 = H % 10;
    }

    rgb_set(humidity);
  }

  // continuous multiplexing (like AVR)
  displayAll(t1, t2, h1, h2);
}
