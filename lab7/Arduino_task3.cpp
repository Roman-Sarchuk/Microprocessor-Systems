const  int dataPin  = 6;
const  int clockPin = 7;
const  int latchPin = 10;
const  int Dig1 = 5;
const  int Dig2 = 4;
const  int Dig3 = 3;
const  int Dig4 = 2;
const  int RED_PIN   = A1;
const  int GREEN_PIN = A2;
const  int BLUE_PIN  = A3;

const  int LM35_PIN = A0;

volatile  int current_digit = 1;
volatile int temp10 = 0;

const  int digitCode[11] = {
  0x3F, // 0
  0x06, // 1  
  0x5B, // 2
  0x4F, // 3
  0x66, // 4
  0x6D, // 5
  0x7D, // 6
  0x07, // 7
  0x7F, // 8
  0x6F, // 9
  0x00  // blank
};

void sendToShift( int data) {
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, data);
  digitalWrite(latchPin, HIGH);
}

ISR(TIMER1_OVF_vect) {
  digitalWrite(Dig1, LOW);
  digitalWrite(Dig2, LOW);
  digitalWrite(Dig3, LOW);
  digitalWrite(Dig4, LOW);

   int digitValue;
  bool showDP = false;

  switch (current_digit) {
    case 1:
      digitValue = (temp10 / 100) % 10;
      showDP = false;
      digitalWrite(Dig1, HIGH);
      break;
    case 2:
      digitValue = (temp10 / 10) % 10;
      showDP = true;
      digitalWrite(Dig2, HIGH);
      break;
    case 3:
      digitValue = temp10 % 10;
      showDP = false;
      digitalWrite(Dig3, HIGH);
      break;
    case 4:
      digitValue = 10;
      showDP = false;
      digitalWrite(Dig4, HIGH);
      break;
  }

  int code = digitCode[digitValue];
  if (showDP) {
    code |= 0x80;
  }
  
  sendToShift(code);
  current_digit = (current_digit % 4) + 1;
}

void setup() {
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(latchPin, OUTPUT);
  
  pinMode(Dig1, OUTPUT);
  pinMode(Dig2, OUTPUT);
  pinMode(Dig3, OUTPUT);
  pinMode(Dig4, OUTPUT);

  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);

  digitalWrite(Dig1, LOW);
  digitalWrite(Dig2, LOW);
  digitalWrite(Dig3, LOW);
  digitalWrite(Dig4, LOW);

  TCCR1A = 0;
  TCCR1B = 1;
  TCNT1 = 0;
  TIMSK1 = 1;
}

void loop() {
  int raw = analogRead(LM35_PIN);
  float tempC = (raw * 5.0 * 100.0) / 1023.0;
  temp10 = (int)(tempC * 10.0 + 0.5);

  if (temp10 < 0) temp10 = 0;
  if (temp10 > 999) temp10 = 999;

  if (tempC >= 33.0) {
    digitalWrite(RED_PIN, HIGH); 
    digitalWrite(GREEN_PIN, LOW); 
    digitalWrite(BLUE_PIN, LOW);
  } else if (tempC >= 25.0) {
    digitalWrite(RED_PIN, LOW); 
    digitalWrite(GREEN_PIN, HIGH); 
    digitalWrite(BLUE_PIN, LOW);
  } else if (tempC > 18.0) {
    digitalWrite(RED_PIN, LOW); 
    digitalWrite(GREEN_PIN, LOW); 
    digitalWrite(BLUE_PIN, HIGH);
  } else {
    digitalWrite(RED_PIN, LOW); 
    digitalWrite(GREEN_PIN, LOW); 
    digitalWrite(BLUE_PIN, LOW);
  }

  delay(300);
}