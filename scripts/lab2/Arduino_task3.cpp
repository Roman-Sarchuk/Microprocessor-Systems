const int BUZ_PIN = 7;     // Buzzer pin

void setup() {
  pinMode(BUZ_PIN, OUTPUT);
  
  pinMode(A1,	INPUT);
}

void loop() {
  int freq = analogRead(A1);
  tone(BUZ_PIN, freq);
  delay(100);
}
