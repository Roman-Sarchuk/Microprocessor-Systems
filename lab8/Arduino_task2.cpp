#include <DHT.h>

// PIN CONFIG
const int SEG_A = 2;
const int SEG_B = 3;
const int SEG_C = 4;
const int SEG_D = 5;
const int SEG_E = 6;
const int SEG_F = 7;
const int SEG_G = 8;
const int SEG_DP = 13;

const int DIG1 = 9;
const int DIG2 = 10;
const int DIG3 = 11;
const int DIG4 = 12;

const int RED_PIN = A1;
const int GREEN_PIN = A2;
const int BLUE_PIN = A3;

const int DHT_PIN = A4;
const int DHT_TYPE = DHT11;

DHT dht(DHT_PIN, DHT_TYPE);

// 7-seg digit codes (common cathode)
// a b c d e f g
const uint8_t digitCode[10] = {
  // gfedcba
  0b00111111, // 0
  0b00000110, // 1
  0b01011011, // 2
  0b01001111, // 3
  0b01100110, // 4
  0b01101101, // 5
  0b01111101, // 6
  0b00000111, // 7
  0b01111111, // 8
  0b01101111  // 9
};

// Letter "t"
const uint8_t LETTER_t = 
  0b01111000;  // using f,e,d,g segments

// Letter "h"
const uint8_t LETTER_h =
  0b01110100;  // using f,e,d,c segments

// Function to set segments from code
// bit0→A ... bit6→G
void setSegments(uint8_t code) {
  digitalWrite(SEG_A, code & 0x01);
  digitalWrite(SEG_B, code & 0x02);
  digitalWrite(SEG_C, code & 0x04);
  digitalWrite(SEG_D, code & 0x08);
  digitalWrite(SEG_E, code & 0x10);
  digitalWrite(SEG_F, code & 0x20);
  digitalWrite(SEG_G, code & 0x40);
}

// Display one character in given digit
void showDigit(uint8_t digit, uint8_t code) {
  // turn all off
  digitalWrite(DIG1, LOW);
  digitalWrite(DIG2, LOW);
  digitalWrite(DIG3, LOW);
  digitalWrite(DIG4, LOW);

  setSegments(code);

  switch(digit) {
    case 1: digitalWrite(DIG1, HIGH); break;
    case 2: digitalWrite(DIG2, HIGH); break;
    case 3: digitalWrite(DIG3, HIGH); break;
    case 4: digitalWrite(DIG4, HIGH); break;
  }

  delay(3); // multiplex delay
}

// show 4 characters in multiplex for ms time
void show4(uint8_t c1, uint8_t c2, uint8_t c3, uint8_t c4, uint16_t ms) {
  uint32_t t0 = millis();
  while(millis() - t0 < ms) {
    showDigit(1, c1);
    showDigit(2, c2);
    showDigit(3, c3);
    showDigit(4, c4);
  }
}

// RGB control based on humidity
void rgb_set(float h) {
  if (h < 40) {
    // Blue
    digitalWrite(RED_PIN, LOW);
    digitalWrite(GREEN_PIN, LOW);
    digitalWrite(BLUE_PIN, HIGH);
  }
  else if (h >= 40 && h <= 60) {
    // Green
    digitalWrite(RED_PIN, LOW);
    digitalWrite(GREEN_PIN, HIGH);
    digitalWrite(BLUE_PIN, LOW);
  }
  else {
    // Red
    digitalWrite(RED_PIN, HIGH);
    digitalWrite(GREEN_PIN, LOW);
    digitalWrite(BLUE_PIN, LOW);
  }
}

// SETUP
void setup() {
  dht.begin();

  // segment pins
  pinMode(SEG_A, OUTPUT);
  pinMode(SEG_B, OUTPUT);
  pinMode(SEG_C, OUTPUT);
  pinMode(SEG_D, OUTPUT);
  pinMode(SEG_E, OUTPUT);
  pinMode(SEG_F, OUTPUT);
  pinMode(SEG_G, OUTPUT);
  pinMode(SEG_DP, OUTPUT);

  // digits
  pinMode(DIG1, OUTPUT);
  pinMode(DIG2, OUTPUT);
  pinMode(DIG3, OUTPUT);
  pinMode(DIG4, OUTPUT);

  // rgb
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);

  digitalWrite(SEG_DP, LOW); // decimal point off
}

// LOOP
void loop() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    // Show ----
    show4(0,0,0,0,500);
    return;
  }

  // convert to integer with 3 digits max
  int tempVal = (int)(t + 0.5);  // nearest integer
  int humVal  = (int)(h + 0.5);

  if (tempVal < 0) tempVal = 0;
  if (tempVal > 999) tempVal = 999;
  if (humVal < 0) humVal = 0;
  if (humVal > 999) humVal = 999;

  // update RGB by humidity
  rgb_set(h);

  // Show temperature tXXX for 1 second
  uint8_t d1 = LETTER_t;

  uint8_t d2 = digitCode[(tempVal / 100) % 10];
  uint8_t d3 = digitCode[(tempVal / 10) % 10];
  uint8_t d4 = digitCode[tempVal % 10];

  show4(d1, d2, d3, d4, 600);

  // Show humidity hXXX for 1 second
  d1 = LETTER_h;

  d2 = digitCode[(humVal / 100) % 10];
  d3 = digitCode[(humVal / 10) % 10];
  d4 = digitCode[humVal % 10];

  show4(d1, d2, d3, d4, 600);
}
