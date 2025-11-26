#include <DHT.h>

// PIN DEFINITIONS
const int DIG1 = 5;
const int DIG2 = 4;
const int DIG3 = 3;
const int DIG4 = 2;

const int DATA_PIN = 6;
const int CLOCK_PIN = 7;
const int LATCH_PIN = 10;

const int RED_PIN = A1;
const int GREEN_PIN = A2;
const int BLUE_PIN = A3;

const int DHT_PIN = A4;
const int DHT_TYPE = DHT11;

DHT dht(DHT_PIN, DHT_TYPE);

// digit codes for 74HC595
// Segments:  DP G F E D C B A
// (common cathode)
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

// letter 't' (segments: f,e,d,g)
const uint8_t LETTER_t = 
  0b01111000;

// letter 'h' (segments: f,e,d,c)
const uint8_t LETTER_h = 
  0b01110100;


// Send byte to 74HC595
void shiftSend(uint8_t data) {
  digitalWrite(LATCH_PIN, LOW);
  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, data);
  digitalWrite(LATCH_PIN, HIGH);
}


// Display single digit
void showDigit(uint8_t digit, uint8_t code) {
  // turn all digits off
  digitalWrite(DIG1, LOW);
  digitalWrite(DIG2, LOW);
  digitalWrite(DIG3, LOW);
  digitalWrite(DIG4, LOW);

  shiftSend(code);

  switch (digit) {
    case 1: digitalWrite(DIG1, HIGH); break;
    case 2: digitalWrite(DIG2, HIGH); break;
    case 3: digitalWrite(DIG3, HIGH); break;
    case 4: digitalWrite(DIG4, HIGH); break;
  }

  delay(3); // multiplex
}


// Show 4 digits for ms
void show4(uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4, uint16_t ms) {
  uint32_t t0 = millis();
  while (millis() - t0 < ms) {
    showDigit(1, d1);
    showDigit(2, d2);
    showDigit(3, d3);
    showDigit(4, d4);
  }
}


// RGB by humidity
void rgb_set(float h) {
  if (h < 40) {
    digitalWrite(RED_PIN, LOW);
    digitalWrite(GREEN_PIN, LOW);
    digitalWrite(BLUE_PIN, HIGH);  // blue
  }
  else if (h >= 40 && h <= 60) {
    digitalWrite(RED_PIN, LOW);
    digitalWrite(GREEN_PIN, HIGH); // green
    digitalWrite(BLUE_PIN, LOW);
  }
  else {
    digitalWrite(RED_PIN, HIGH);   // red
    digitalWrite(GREEN_PIN, LOW);
    digitalWrite(BLUE_PIN, LOW);
  }
}


// SETUP
void setup() {
  dht.begin();

  pinMode(DATA_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(LATCH_PIN, OUTPUT);

  pinMode(DIG1, OUTPUT);
  pinMode(DIG2, OUTPUT);
  pinMode(DIG3, OUTPUT);
  pinMode(DIG4, OUTPUT);

  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);

  digitalWrite(RED_PIN, LOW);
  digitalWrite(GREEN_PIN, LOW);
  digitalWrite(BLUE_PIN, LOW);
}


// LOOP
void loop() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    show4(0,0,0,0,500);
    return;
  }

  // convert to integer 0...999
  int tempVal = (int)(t + 0.5);
  int humVal  = (int)(h + 0.5);

  if (tempVal < 0) tempVal = 0;
  if (tempVal > 999) tempVal = 999;

  if (humVal < 0) humVal = 0;
  if (humVal > 999) humVal = 999;

  // RGB update by humidity
  rgb_set(h);

  // Show temperature tXXX
  uint8_t d1 = LETTER_t;
  uint8_t d2 = digitCode[(tempVal / 100) % 10];
  uint8_t d3 = digitCode[(tempVal / 10) % 10];
  uint8_t d4 = digitCode[tempVal % 10];

  show4(d1, d2, d3, d4, 1000);

  // Show humidity hXXX
  d1 = LETTER_h;
  d2 = digitCode[(humVal / 100) % 10];
  d3 = digitCode[(humVal / 10) % 10];
  d4 = digitCode[humVal % 10];

  show4(d1, d2, d3, d4, 1000);
}
