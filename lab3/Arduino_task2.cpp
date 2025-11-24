#define BLED 9
#define GLED 10
#define RLED 11
#define BTN_COLOR 2
#define BTN_FREQUENCY 3

volatile int ledMode = 0;
volatile int freq = 50;

volatile unsigned long lastInterruptColor = 0;
volatile unsigned long lastInterruptFreq = 0;


void setup() {
  pinMode (BLED, OUTPUT); 
  pinMode (GLED, OUTPUT); 
  pinMode (RLED, OUTPUT);
  pinMode (BTN_COLOR, INPUT_PULLUP);
  pinMode (BTN_FREQUENCY, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(BTN_COLOR), changeMode, RISING);
  attachInterrupt(digitalPinToInterrupt(BTN_FREQUENCY), changeFrequency, RISING);
}


void loop() {
  setMode(ledMode);
  delay(freq);
  turnOff(ledMode);
  delay(freq);
}


void setMode(int mode) {
  switch (mode) {
    case 1: // Red
      digitalWrite(RLED, HIGH);
      digitalWrite(GLED, LOW);
      digitalWrite(BLED, LOW);
      break;

    case 2: // Green
      digitalWrite(RLED, LOW);
      digitalWrite(GLED, HIGH);
      digitalWrite(BLED, LOW);
      break;

    case 3: // Blue
      digitalWrite(RLED, LOW);
      digitalWrite(GLED, LOW);
      digitalWrite(BLED, HIGH);
      break;

    case 4: // Purple (Red + Blue)
      analogWrite(RLED, 127);
      analogWrite(GLED, 0);
      analogWrite(BLED, 127);
      break;

    case 5: // Turquoise (Blue + Green)
      analogWrite(RLED, 0);
      analogWrite(GLED, 127);
      analogWrite(BLED, 127);
      break;

    case 6: // Orange (Red + Green)
      analogWrite(RLED, 127);
      analogWrite(GLED, 127);
      analogWrite(BLED, 0);
      break;

    case 7: // White (Red + Green + Blue)
      analogWrite(RLED, 85);
      analogWrite(GLED, 85);
      analogWrite(BLED, 85);
      break;

    default: // 0 or others
      digitalWrite(RLED, LOW);
      digitalWrite(GLED, LOW);
      digitalWrite(BLED, LOW);
      break;
  }
}


void turnOff(int mode) {
  if (mode >= 4 && mode <= 7) {
    analogWrite(RLED, 0);
    analogWrite(GLED, 0);
    analogWrite(BLED, 0);
  } else {
    digitalWrite(RLED, LOW);
    digitalWrite(GLED, LOW);
    digitalWrite(BLED, LOW);
  }
}


void changeMode() {
  unsigned long interrupt_time = millis();

  if (interrupt_time - lastInterruptColor > 200) {
    ledMode++;
    if (ledMode == 8) 
      ledMode = 0;
  }

  lastInterruptColor = interrupt_time;
}


void changeFrequency() {
  unsigned long interrupt_time = millis();

  if (interrupt_time - lastInterruptFreq > 200) {
    freq += 100;
    if (freq > 400) {
      freq = 50;
    }
  }

  lastInterruptFreq = interrupt_time;
}
