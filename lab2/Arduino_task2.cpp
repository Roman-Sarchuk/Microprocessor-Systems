const int BUZ_PIN = 7;     // Buzzer pin
volatile int freq = 500;   // Initial frequency
const int FREQ_STEP = 100;  // Frequency change step
const int FREQ_MIN = 100;  // Minimum frequency
const int FREQ_MAX = 2000; // Maximum frequency

void setup() {
  pinMode(BUZ_PIN, OUTPUT);
  
  pinMode(2, INPUT_PULLUP);  // INT0
  attachInterrupt(digitalPinToInterrupt(2), increaseFreq, FALLING);
  
  pinMode(3, INPUT_PULLUP);  // INT1
  attachInterrupt(digitalPinToInterrupt(3), decreaseFreq, FALLING);
}

void loop() {
  tone(BUZ_PIN, freq);
  delay(50);
}

void increaseFreq() {
  freq += FREQ_STEP;
  if (freq > FREQ_MAX) freq = FREQ_MAX;
}

void decreaseFreq() {
  freq -= FREQ_STEP;
  if (freq < FREQ_MIN) freq = FREQ_MIN;
}
