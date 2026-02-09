#define BLED 9
#define GLED 10 
#define RLED 11
#define BUTTON 2

volatile int ledMode = 0;
volatile unsigned long last_interrupt_time = 0;


void setup() {
  pinMode(BLED, OUTPUT); 
  pinMode(GLED, OUTPUT); 
  pinMode(RLED, OUTPUT);
  pinMode(BUTTON, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON), changeMode, RISING);
} 


void loop() {
  setMode(ledMode);
  delay(20);
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


void changeMode() {
  unsigned long interrupt_time = millis();

  if (interrupt_time - last_interrupt_time > 200) {
    ledMode++;
    if (ledMode == 8) 
      ledMode = 0;
  }

  last_interrupt_time = interrupt_time;
}
