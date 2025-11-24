const short BTN_PIN_COUNT = 4;
const int BTN_PINS[BTN_PIN_COUNT] = {2, 3, 4, 5};
const int BTN_FREQUENCY[BTN_PIN_COUNT] = {250, 500, 750, 1000};
const int BUZ_PIN = 7;

void setup() {
  for (int i = 0; i < BTN_PIN_COUNT; i++) {
    pinMode(BTN_PINS[i], INPUT_PULLUP);
  }
  pinMode(BUZ_PIN, OUTPUT);
}

void loop() {
  for (int i = 0; i < BTN_PIN_COUNT; i++) {
    if (digitalRead(BTN_PINS[i]) == LOW) {
      tone(BUZ_PIN, BTN_FREQUENCY[i], 300);   // play tone on buzzer
      delay(300);                             // wait for the duration
      noTone(BUZ_PIN);                        // stop tone
    }
  }
}