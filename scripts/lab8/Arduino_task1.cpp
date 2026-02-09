#include <LiquidCrystal.h>
#include <DHT.h>

// LCD pins
const int LCD_RS = 12;
const int LCD_E  = 11;
const int LCD_D4 = 5;
const int LCD_D5 = 4;
const int LCD_D6 = 3;
const int LCD_D7 = 2;

LiquidCrystal lcd(LCD_RS, LCD_E, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

// DHT11
const int DHT_PIN = 7;
const int DHT_TYPE = DHT11;
DHT dht(DHT_PIN, DHT_TYPE);

// RGB LED pins
const int RED_PIN   = 8;
const int GREEN_PIN = 9;
const int BLUE_PIN  = 10;

void setup() {
  lcd.begin(16, 2);
  lcd.clear();

  dht.begin();

  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);

  digitalWrite(RED_PIN, LOW);
  digitalWrite(GREEN_PIN, LOW);
  digitalWrite(BLUE_PIN, LOW);
}

void loop() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  // Reading error
  if (isnan(h) || isnan(t)) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Sensor error");
    delay(1000);
    return;
  }

  // LCD control
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("T: ");
  lcd.print(t, 1);
  lcd.print((char)223);
  lcd.print("C");

  lcd.setCursor(0, 1);
  lcd.print("H: ");
  lcd.print(h, 1);
  lcd.print("%");

  // RGB control
  if (t <= 18) {
    // Off
    digitalWrite(RED_PIN, LOW);
    digitalWrite(GREEN_PIN, LOW);
    digitalWrite(BLUE_PIN, LOW);
  }
  else if (t < 25) {
    // Blue
    digitalWrite(RED_PIN, LOW);
    digitalWrite(GREEN_PIN, LOW);
    digitalWrite(BLUE_PIN, HIGH);
  }
  else if (t < 33) {
    // Green
    digitalWrite(RED_PIN, LOW);
    digitalWrite(GREEN_PIN, HIGH);
    digitalWrite(BLUE_PIN, LOW);
  }
  else {
    // Red
    digitalWrite(RED_PIN, HIGH);
    digitalWrite(GREEN_PIN, LOW);
    digitalWrite(BLUE_PIN, LOW);
  }

  delay(500);
}
