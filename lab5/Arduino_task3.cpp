#define btn1 A0
#define btn2 A2
#define btn3 A3
#define btn4 A4

#define clockPin 7
#define dataPin 6
#define latchPin 8

#define Dig1 5
#define Dig2 4
#define Dig3 3
#define Dig4 2

byte digits[4] = {0, 0, 0, 0};  
bool btn1_prev = HIGH, btn2_prev = HIGH, btn3_prev = HIGH, btn4_prev = HIGH;

byte numeral[10] = {
  B11111100, // 0
  B01100000, // 1
  B11011010, // 2
  B11110010, // 3
  B01100110, // 4
  B10110110, // 5
  B00111110, // 6
  B11100000, // 7
  B11111110, // 8
  B11100110  // 9
};

void setup() {
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(latchPin, OUTPUT);

  pinMode(Dig1, OUTPUT);
  pinMode(Dig2, OUTPUT);
  pinMode(Dig3, OUTPUT);
  pinMode(Dig4, OUTPUT);

  pinMode(btn1, INPUT_PULLUP);
  pinMode(btn2, INPUT_PULLUP);
  pinMode(btn3, INPUT_PULLUP);
  pinMode(btn4, INPUT_PULLUP);
 
  digitalWrite(Dig1, LOW);
  digitalWrite(Dig2, LOW);
  digitalWrite(Dig3, LOW);
  digitalWrite(Dig4, LOW);
}

void loop() {
  handleButton(btn1, btn1_prev, digits[0]);
  handleButton(btn2, btn2_prev, digits[1]);
  handleButton(btn3, btn3_prev, digits[2]);
  handleButton(btn4, btn4_prev, digits[3]);

  showDigit(0, digits[0]);
  delay(3);
  showDigit(1, digits[1]);
  delay(3);
  showDigit(2, digits[2]);
  delay(3);
  showDigit(3, digits[3]);
  delay(3);
}

void handleButton(int btn_pin, bool &prev_state, byte &digit) {
  bool current_state = digitalRead(btn_pin);
 
  if (current_state == LOW && prev_state == HIGH) {
    delay(50);
    digit = (digit + 1) % 10;
  }
  prev_state = current_state;
}

void showDigit(byte digit_pos, byte num) {
  digitalWrite(Dig1, LOW);
  digitalWrite(Dig2, LOW);
  digitalWrite(Dig3, LOW);
  digitalWrite(Dig4, LOW);
 
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, numeral[num]);
  digitalWrite(latchPin, HIGH);
 
  switch(digit_pos) {
    case 0: digitalWrite(Dig1, HIGH); break;
    case 1: digitalWrite(Dig2, HIGH); break;
    case 2: digitalWrite(Dig3, HIGH); break;
    case 3: digitalWrite(Dig4, HIGH); break;
  }
}