#define BLED 9 
#define GLED 10 
#define RLED 11 
#define POT A0


void setup() {
  pinMode(RLED, OUTPUT);
  pinMode(GLED, OUTPUT);
  pinMode(BLED, OUTPUT);
  pinMode(A1, INPUT);
}

void loop() {
  int val = analogRead(POT);
  
  // Более интересные цветовые переходы
  int red = map(val, 0, 1023, 0, 255);
  int green = 255 - red;
  int blue = red / 2;
  
  analogWrite(RLED, red);
  analogWrite(GLED, green);
  analogWrite(BLED, blue);
  delay(5);
}