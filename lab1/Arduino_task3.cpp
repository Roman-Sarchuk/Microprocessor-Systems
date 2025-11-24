// Lab1.p3 - runing Fire/Shadow
int leds[] = {9, 5, 7, 10, 2, 3};
int numLeds = 6;
int delayTime = 200; // speed "running" (ms)

void setup() {
  // setup pins
  for (int i = 0; i < numLeds; i++) {
    pinMode(leds[i], OUTPUT);
    digitalWrite(leds[i], LOW);
  }
}

void loop() {
  // fire
  turnOffAll();
  for (int i = 0; i < numLeds; i++) {
    digitalWrite(leds[i], HIGH);
    delay(delayTime);
    digitalWrite(leds[i], LOW);
  }
  delay(delayTime);

  // shadow
  turnOnAll();
  for (int i = numLeds; i > 0; i--) {
    digitalWrite(leds[i], LOW);
    delay(delayTime);
    digitalWrite(leds[i], HIGH);
  }
  delay(delayTime);
}

void turnOnAll() {
  for (int i = 0; i < numLeds; i++) {
    digitalWrite(leds[i], HIGH);
  }
}

void turnOffAll() {
  for (int i = 0; i < numLeds; i++) {
    digitalWrite(leds[i], LOW);
  }
}
