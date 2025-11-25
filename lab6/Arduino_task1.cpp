#include <LiquidCrystal.h>

const int RS = 8;
const int E = 9;
const int D4 = 4;
const int D5 = 5;
const int D6 = 6;
const int D7 = 7;

LiquidCrystal lcd(RS, E, D4, D5, D6, D7);

const char fullName[] = "Sarchuk Roman Andriovych ";
const int len = sizeof(fullName) - 1;
const int width = 16;

volatile int pos = 0;

void setup() {
  lcd.begin(16, 2);

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

void loop() {}
