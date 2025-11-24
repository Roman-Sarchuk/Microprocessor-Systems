// Lab1.p1 - running fire
int leds[] = {9, 5, 7, 10, 1, 2};
int numLeds = 6;
int delayTime = 200; // speed "running" (ms)
int curPin = 0;

void setup() {
  // setup pins
  for (int i = 0; i < numLeds; i++) {
    pinMode(leds[i], OUTPUT);
    digitalWrite(leds[i], LOW);
  }
}

void loop() {
  digitalWrite(leds[curPin], HIGH);
  delay(delayTime);
  digitalWrite(leds[curPin], LOW);
  delay(delayTime/3);

  curPin++;
  if (curPin >= numLeds) {
    curPin = 0;
  }
}
