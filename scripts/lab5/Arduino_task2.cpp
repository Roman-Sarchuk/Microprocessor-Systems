#define POT A1
#define button A0

#define clockPin 7
#define dataPin 6  
#define latchPin 8

#define Dig1 5
#define Dig2 4
#define Dig3 3
#define Dig4 2

byte current_digit = 1;
int display_value = 0;
bool last_button_state = HIGH;

void disp(byte number);
void disp_off();

void setup() {
  pinMode(button, INPUT_PULLUP);
  pinMode(Dig1, OUTPUT);
  pinMode(Dig2, OUTPUT);
  pinMode(Dig3, OUTPUT);
  pinMode(Dig4, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(latchPin, OUTPUT);

  randomSeed(analogRead(POT));  
  display_value = random(0, 10000);
 
  disp_off();

  TCCR1A = 0;
  TCCR1B = 1;
  TCNT1 = 0;
  TIMSK1 = 1;
}

ISR(TIMER1_OVF_vect) {
  disp_off();

  switch (current_digit) {
    case 1:
      disp(display_value / 1000);
      digitalWrite(Dig1, HIGH);
      break;
    case 2:
      disp((display_value / 100) % 10);
      digitalWrite(Dig2, HIGH);
      break;
    case 3:
      disp((display_value / 10) % 10);
      digitalWrite(Dig3, HIGH);
      break;
    case 4:
      disp(display_value % 10);
      digitalWrite(Dig4, HIGH);
      break;
  }

  current_digit = (current_digit % 4) + 1;
}

void loop() {
  bool current_button_state = digitalRead(button);
 
  if (last_button_state == HIGH && current_button_state == LOW) {
    delay(20);  
    if (digitalRead(button) == LOW) {
      display_value = random(0, 10000);
    }
  }
 
  last_button_state = current_button_state;
}

void disp(byte number) {
  digitalWrite(latchPin, LOW);
  switch (number) {
    case 0: shiftOut(dataPin, clockPin, MSBFIRST, 0xFC); break;
    case 1: shiftOut(dataPin, clockPin, MSBFIRST, 0x60); break;
    case 2: shiftOut(dataPin, clockPin, MSBFIRST, 0xDA); break;
    case 3: shiftOut(dataPin, clockPin, MSBFIRST, 0xF2); break;
    case 4: shiftOut(dataPin, clockPin, MSBFIRST, 0x66); break;
    case 5: shiftOut(dataPin, clockPin, MSBFIRST, 0xB6); break;
    case 6: shiftOut(dataPin, clockPin, MSBFIRST, 0xBE); break;
    case 7: shiftOut(dataPin, clockPin, MSBFIRST, 0xE0); break;
    case 8: shiftOut(dataPin, clockPin, MSBFIRST, 0xFE); break;
    case 9: shiftOut(dataPin, clockPin, MSBFIRST, 0xF6); break;
  }
  digitalWrite(latchPin, HIGH);
}

void disp_off() {
  digitalWrite(Dig1, LOW);
  digitalWrite(Dig2, LOW);
  digitalWrite(Dig3, LOW);
  digitalWrite(Dig4, LOW);
}