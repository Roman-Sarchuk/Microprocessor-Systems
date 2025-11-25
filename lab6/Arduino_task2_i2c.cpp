#include <LiquidCrystal.h>
#include <Wire.h>
#include <RTClib.h>

const int RS = 8;
const int E = 9;
const int D4 = 4;
const int D5 = 5;
const int D6 = 6;
const int D7 = 7;

RTC_DS1307 rtc;
LiquidCrystal lcd(RS, E, D4, D5, D6, D7);

const char fullName[] = "Sarchuk Roman Andriovych ";
const int len = sizeof(fullName) - 1;
const int width = 16;

volatile int pos = 0;

void setup() {
  lcd.begin(16, 2);

  if (!rtc.begin()) {
    lcd.print("RTC not found");
    while (1);
  }

  if (!rtc.isrunning()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  cli();
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;

  OCR1A = 6249;
  TCCR1B |= (1 << WGM12);
  TCCR1B |= (1 << CS12) | (1 << CS10);
  TIMSK1 |= (1 << OCIE1A);

  sei();
}

ISR(TIMER1_COMPA_vect) {
  lcd.setCursor(0, 0);
  for (int i = 0; i < width; i++) {
    int index = (pos + i) % len;
    lcd.write(fullName[index]);
  }
  pos++;
  if (pos >= len) pos = 0;
}

void loop() {
  DateTime now = rtc.now();
  
  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print("Date: ");
  lcd.print(now.day());
  lcd.print('.');
  lcd.print(now.month());
  lcd.print('.');
  lcd.print(now.year());
  delay(1000);

  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print("Time: ");
  if (now.hour() < 10) lcd.print(0);
  lcd.print(now.hour());
  lcd.print(":");
  if (now.minute() < 10) lcd.print(0);
  lcd.print(now.minute());
  delay(1000);
}
