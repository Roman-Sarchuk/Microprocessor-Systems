const short LED_COUNT = 6;
const int LED_PINS[LED_COUNT] = {3, 5, 6, 9, 10, 11};
int ledBrightness[LED_COUNT] = {70, 100, 130, 180, 220, 255};

void setup() {
  // set up pins
  for (short i = 0; i < LED_COUNT; i++) {
    pinMode(LED_PINS[i], OUTPUT);
  }

  // write
  for (short i = 0; i < LED_COUNT; i++) {
    analogWrite(LED_PINS[i], ledBrightness[i]);
  }
}

void loop() {

}