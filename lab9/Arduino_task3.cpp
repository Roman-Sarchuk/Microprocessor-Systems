#include <Wire.h>
const uint8_t segPins[7] = {2, 3, 4, 5, 6, 7, 8};
const uint8_t dpPin = 13;
const uint8_t digPins[4] = {9, 10, 11, 12}; 

int clockData[7];
unsigned long lastModeChange = 0;
const unsigned long MODE_INTERVAL = 500UL;
uint8_t mode = 0;

uint8_t currentDigit = 0;

const uint8_t segDigitMap[10] = {
  0x3F, // 0 -> 0b00111111
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

const uint8_t CHAR_D = 0x5E; 
const uint8_t CHAR_A = 0x77; 
const uint8_t CHAR_t = 0x78; 
const uint8_t CHAR_E = 0x79; 
const uint8_t CHAR_I = 0x06; 
const uint8_t CHAR_n = 0x37;



byte bcdToDec(byte val) { return (val / 16 * 10) + (val % 16); }
void DS1307_get(int *timeArray) {
  Wire.beginTransmission(0x68);
  Wire.write(0x00);
  Wire.endTransmission();
  Wire.requestFrom(0x68, 7);
  for (int i = 0; i < 7 && Wire.available(); ++i) {
    timeArray[i] = bcdToDec(Wire.read());
  }
}

void setSegments(uint8_t pattern, bool dpOn) {
  for (uint8_t i = 0; i < 7; ++i) {
    digitalWrite(segPins[i], (pattern & (1 << i)) ? HIGH : LOW);
  }
  digitalWrite(dpPin, dpOn ? HIGH : LOW);
}

void switchOffDigits() {
  for (uint8_t i = 0; i < 4; ++i) digitalWrite(digPins[i], LOW);
}

void showCodeOnDigit(uint8_t pos, int digitCode, bool dp) {
  switchOffDigits();

  uint8_t pat = 0;
  if (digitCode >= 0 && digitCode <= 9) {
    pat = segDigitMap[digitCode];
  } else {
    switch (digitCode) {
      case 100: pat = CHAR_D; break; // 'D'
      case 101: pat = CHAR_A; break; // 'A'
      case 102: pat = CHAR_t; break; // 't'
      case 103: pat = CHAR_E; break; // 'E'
      case 104: pat = CHAR_I; break; // 'I'
      case 105: pat = CHAR_n; break; // 'n' 
      default: pat = 0x00; break;
    }
  }

  setSegments(pat, dp);

  digitalWrite(digPins[pos], HIGH);

  delay(2);
  digitalWrite(digPins[pos], LOW);
}

// ---------- setup ----------
void setup() {
  Wire.begin();
  for (uint8_t i = 0; i < 7; ++i) pinMode(segPins[i], OUTPUT);
  pinMode(dpPin, OUTPUT);
  for (uint8_t i = 0; i < 4; ++i) pinMode(digPins[i], OUTPUT);

  switchOffDigits();
  for (uint8_t i = 0; i < 7; ++i) digitalWrite(segPins[i], LOW);
  digitalWrite(dpPin, LOW);

  lastModeChange = millis();
}

int displayBuffer[4]; 
bool dpBuffer[4];     

void prepareBufferForMode(uint8_t modeIndex) {
  // очистити
  for (uint8_t i = 0; i < 4; ++i) { displayBuffer[i] = -1; dpBuffer[i] = false; }

  if (modeIndex == 0) {
    displayBuffer[0] = 100; // 'd'
    displayBuffer[1] = 101; // 'A'
    displayBuffer[2] = 102; // 't'
    displayBuffer[3] = 103; // 'E'
  }
  else if (modeIndex == 1) {
    int day = clockData[4];
    int month = clockData[5];
    displayBuffer[0] = day / 10;
    displayBuffer[1] = day % 10;
    displayBuffer[2] = month / 10;
    displayBuffer[3] = month % 10;


    dpBuffer[1] = true;
  }
  else if (modeIndex == 2) {
    int year = 2000 + clockData[6];
    int y = year;
    displayBuffer[0] = (y / 1000) % 10;
    displayBuffer[1] = (y / 100) % 10;
    displayBuffer[2] = (y / 10) % 10;
    displayBuffer[3] = y % 10;
  }
  else if (modeIndex == 3) {
    displayBuffer[0] = 102; // 't'
    displayBuffer[1] = 104; // 'o' ~ 'i'
    displayBuffer[2] = 105; // 'o' ~ 'm'
    displayBuffer[3] = 103; // 'r' or 'E' 
  }
  else if (modeIndex == 4) {
    int hr = clockData[2];
    int mn = clockData[1];
    displayBuffer[0] = hr / 10;
    displayBuffer[1] = hr % 10;
    displayBuffer[2] = mn / 10;
    displayBuffer[3] = mn % 10;
    dpBuffer[1] = true;
  }
}

// ---------- main loop ----------
void loop() {
  DS1307_get(clockData);

  if (millis() - lastModeChange >= MODE_INTERVAL) {
    lastModeChange = millis();
    mode = (mode + 1) % 5;
    prepareBufferForMode(mode);
  }

  static bool initialized = false;
  if (!initialized) {
    prepareBufferForMode(mode);
    initialized = true;
  }

  if (displayBuffer[currentDigit] == -1) {
    setSegments(0x00, dpBuffer[currentDigit]);
    digitalWrite(digPins[currentDigit], HIGH);
    delay(2);
    digitalWrite(digPins[currentDigit], LOW);
  } else {
    showCodeOnDigit(currentDigit, displayBuffer[currentDigit], dpBuffer[currentDigit]);
  }

  currentDigit = (currentDigit + 1) % 4;
}
