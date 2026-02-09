#include <Wire.h>
#include <LiquidCrystal.h>

const int RS = 8;
const int E = 9;
const int D4 = 4;
const int D5 = 5;
const int D6 = 6;
const int D7 = 7;

LiquidCrystal lcd(RS, E, D4, D5, D6, D7);

int clockData[7];

// --- Alarm clock setup ---
int alarmHour = 9; 
int alarmMinute = 17;    

const int buzzer = 3; 
const int ledPin = 2; 

bool alarmTriggered = false;
byte decToBcd(byte val) { return ((val / 10 * 16) + (val % 10)); }
byte bcdToDec(byte val) { return ((val / 16 * 10) + (val % 16)); }

void DS1307_get(int *t) {
  Wire.beginTransmission(0x68);
  Wire.write(0x00);
  Wire.endTransmission();

  Wire.requestFrom(0x68, 7);
  for (int i = 0; i < 7; i++)
    t[i] = bcdToDec(Wire.read());
}

void DS1307_set(int year, int month, int day, int dow, int hour, int minute, int second) {
  Wire.beginTransmission(0x68);
  Wire.write(0x00);
  Wire.write(decToBcd(second));
  Wire.write(decToBcd(minute));
  Wire.write(decToBcd(hour));
  Wire.write(decToBcd(dow));
  Wire.write(decToBcd(day));
  Wire.write(decToBcd(month));
  Wire.write(decToBcd(year));
  Wire.endTransmission();
}

void setup() {
  Wire.begin();
  Serial.begin(9600);

  lcd.begin(16, 2);
  lcd.clear();

  pinMode(buzzer, OUTPUT);
  pinMode(ledPin, OUTPUT);

  digitalWrite(buzzer, LOW);
  digitalWrite(ledPin, LOW);

  //DS1307_set(2025, 12, 3, 1, 13, 43, 0);
}

void loop() {

  DS1307_get(clockData);

  int sec = clockData[0];
  int min = clockData[1];
  int hr  = clockData[2];
  lcd.setCursor(0, 0);
  lcd.print("Now  ");
  print2(hr); lcd.print(":");
  print2(min); lcd.print(":");
  print2(sec);

  lcd.setCursor(0, 1);
  lcd.print("Alarm ");
  print2(alarmHour); lcd.print(":");
  print2(alarmMinute);

  if (hr == alarmHour && min == alarmMinute && !alarmTriggered) {
    alarmTriggered = true;
    triggerAlarm();
  }

  delay(200);
}

void triggerAlarm() {
  Serial.println("ALARM ACTIVATE!");

  for (int i = 0; i < 20; i++) {   
    tone(buzzer, 1000);
    digitalWrite(ledPin, HIGH);
    delay(200);

    noTone(buzzer);
    digitalWrite(ledPin, LOW);
    delay(200);
  }
}

void print2(int n) {
  lcd.print(n / 10);
  lcd.print(n % 10);
}
