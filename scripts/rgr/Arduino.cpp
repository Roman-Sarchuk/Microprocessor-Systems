#include <Wire.h>
#include <RTClib.h>
#include <LiquidCrystal.h>
#include <Adafruit_BMP085.h>

// ==========================
//        Devices
// ==========================
RTC_DS1307 rtc;
Adafruit_BMP085 bmp;

// LCD: your pins
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// ==========================
//        Button Pins
// ==========================
#define BTN_SET_TIME   2
#define BTN_SET_ALARM  3
#define BTN_OK         10
#define BTN_PLUS       11
#define BTN_MINUS      12
#define BTN_STOP       13

const int btnPins[6] = {BTN_SET_TIME, BTN_SET_ALARM, BTN_OK, BTN_PLUS, BTN_MINUS, BTN_STOP};

// ==========================
//        Buzzer/LED
// ==========================
#define BUZ_PIN A0

// ==========================
//        Timing
// ==========================
unsigned long lastClockUpdate = 0;
unsigned long lastSensorUpdate = 0;

const unsigned long CLOCK_INTERVAL = 1000;   // update every 1 sec
const unsigned long SENSOR_INTERVAL = 2000;  // BMP180 every 2 sec

// ==========================
//        Data Storage
// ==========================
DateTime currentTime;
int temperature = 0;
long pressure = 0;

// Alarm
bool alarmEnabled = false;
bool alarmRinging = false;

int alarmYear, alarmMonth, alarmDay;
int alarmHour, alarmMinute;

unsigned long alarmStartTime = 0;

// ==========================
//      Modes
// ==========================
enum {
  MODE_CLOCK,
  MODE_SET_TIME,
  MODE_SET_ALARM,
  MODE_ALARM_RINGING
};

int mode = MODE_CLOCK;

// Values edited in menus:
int editField = 0;
int editYear, editMonth, editDay;
int editHour, editMinute;

// ==========================
//      Button Debounce
// ==========================
bool btnState[6] = {false};
bool lastBtnState[6] = {false};
unsigned long lastDebounceTime[6] = {0};
const unsigned long DEBOUNCE_DELAY = 50;

// ==========================
//      Helper: buttons
// ==========================
bool pressed(int idx) {
  bool reading = digitalRead(btnPins[idx]) == LOW;
  if (reading != lastBtnState[idx]) {
    lastDebounceTime[idx] = millis();
  }

  if ((millis() - lastDebounceTime[idx]) > DEBOUNCE_DELAY) {
    if (reading != btnState[idx]) {
      btnState[idx] = reading;
      if (btnState[idx]) return true; // натискання
    }
  }
  lastBtnState[idx] = reading;
  return false;
}

// ==========================
//      Read Sensors
// ==========================
void readSensorsNow() {
  temperature = bmp.readTemperature();   // °C
  pressure = bmp.readPressure();         // Pa
}

// ==========================
//      Display: Main
// ==========================
void showMainScreen() {
  lcd.clear();

  DateTime now = rtc.now();

  lcd.setCursor(0, 0);
  char line1[17];
  snprintf(line1, 17, "%02d:%02d|%02d.%02d.%04d",
           now.hour(), now.minute(),
           now.day(), now.month(), now.year());
  lcd.print(line1);

  lcd.setCursor(0, 1);
  char line2[17];
  snprintf(line2, 17, "%02dC|%ldPa", temperature, pressure);
  lcd.print(line2);
}

// ==========================
//      Display: Set Time
// ==========================
void showSetTimeMenu() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Set Time&Date");

  lcd.setCursor(0, 1);
  switch (editField) {
    case 0: lcd.print("Year: "); lcd.print(editYear); break;
    case 1: lcd.print("Month: "); lcd.print(editMonth); break;
    case 2: lcd.print("Day: "); lcd.print(editDay); break;
    case 3: lcd.print("Hour: "); lcd.print(editHour); break;
    case 4: lcd.print("Minute: "); lcd.print(editMinute); break;
  }
}

// ==========================
//      Display: Set Alarm
// ==========================
void showSetAlarmMenu() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Set AlarmClock");

  lcd.setCursor(0, 1);
  switch (editField) {
    case 0: lcd.print("Year: "); lcd.print(editYear); break;
    case 1: lcd.print("Month: "); lcd.print(editMonth); break;
    case 2: lcd.print("Day: "); lcd.print(editDay); break;
    case 3: lcd.print("Hour: "); lcd.print(editHour); break;
    case 4: lcd.print("Minute: "); lcd.print(editMinute); break;
  }
}

// ==========================
//     Commit Time
// ==========================
void commitTime() {
  rtc.adjust(DateTime(editYear, editMonth, editDay, editHour, editMinute, 0));

  mode = MODE_CLOCK;
  currentTime = rtc.now();
  readSensorsNow();
  showMainScreen();
}

// ==========================
//     Commit Alarm
// ==========================
void commitAlarm() {
  alarmYear = editYear;
  alarmMonth = editMonth;
  alarmDay = editDay;
  alarmHour = editHour;
  alarmMinute = editMinute;

  alarmEnabled = true;

  mode = MODE_CLOCK;
  currentTime = rtc.now();
  readSensorsNow();
  showMainScreen();
}

// ==========================
//     Stop Alarm
// ==========================
void stopAlarm() {
  noTone(BUZ_PIN);
  alarmRinging = false;
  alarmEnabled = false;

  mode = MODE_CLOCK;
  showMainScreen();
}

// ==========================
//     Alarm Check
// ==========================
void checkAlarm() {
  if (!alarmEnabled) return;

  DateTime now = rtc.now();

  if (now.year() == alarmYear &&
      now.month() == alarmMonth &&
      now.day() == alarmDay &&
      now.hour() == alarmHour &&
      now.minute() == alarmMinute &&
      now.second() == 0)
  {
    alarmRinging = true;
    mode = MODE_ALARM_RINGING;
    alarmStartTime = millis();

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("ALARM RINGING!");
    lcd.setCursor(0, 1);
    lcd.print("STOP BUTTON");

    tone(BUZ_PIN, 1000);  // LED+BUZ together
  }
}

// ==========================
//          SETUP
// ==========================
void setup() {
  lcd.begin(16, 2);

  pinMode(BUZ_PIN, OUTPUT);

  for (int i = 0; i < 6; i++) pinMode(btnPins[i], INPUT_PULLUP);

  rtc.begin();
  bmp.begin();

  currentTime = rtc.now();
  readSensorsNow();
  showMainScreen();
}

// ==========================
//           LOOP
// ==========================
void loop() {

  // ---------- MODE CLOCK ----------
  if (mode == MODE_CLOCK) {

    // clock refresh
    if (millis() - lastClockUpdate >= CLOCK_INTERVAL) {
      lastClockUpdate = millis();
      currentTime = rtc.now();
      showMainScreen();
      checkAlarm();
    }

    // sensor refresh
    if (millis() - lastSensorUpdate >= SENSOR_INTERVAL) {
      lastSensorUpdate = millis();
      readSensorsNow();
      showMainScreen();
    }

    // enter time edit
    if (pressed(0)) { // BTN_SET_TIME
      DateTime now = rtc.now();
      editYear = now.year();
      editMonth = now.month();
      editDay = now.day();
      editHour = now.hour();
      editMinute = now.minute();
      editField = 0;
      mode = MODE_SET_TIME;
      showSetTimeMenu();
    }

    // enter alarm edit
    if (pressed(1)) { // BTN_SET_ALARM
      DateTime now = rtc.now();
      editYear = now.year();
      editMonth = now.month();
      editDay = now.day();
      editHour = now.hour();
      editMinute = now.minute();
      editField = 0;
      mode = MODE_SET_ALARM;
      showSetAlarmMenu();
    }
  }

  // ---------- MODE SET TIME ----------
  else if (mode == MODE_SET_TIME) {

    if (pressed(3)) { // BTN_PLUS
      if (editField == 0) editYear++;
      else if (editField == 1) editMonth = (editMonth % 12) + 1;
      else if (editField == 2) editDay = (editDay % 31) + 1;
      else if (editField == 3) editHour = (editHour + 1) % 24;
      else if (editField == 4) editMinute = (editMinute + 1) % 60;
      showSetTimeMenu();
    }

    if (pressed(4)) { // BTN_MINUS
      if (editField == 0) editYear--;
      else if (editField == 1) editMonth = (editMonth == 1 ? 12 : editMonth - 1);
      else if (editField == 2) editDay = (editDay == 1 ? 31 : editDay - 1);
      else if (editField == 3) editHour = (editHour == 0 ? 23 : editHour - 1);
      else if (editField == 4) editMinute = (editMinute == 0 ? 59 : editMinute - 1);
      showSetTimeMenu();
    }

    if (pressed(2)) { // BTN_OK
      if (editField < 4) {
        editField++;
        showSetTimeMenu();
      } else {
        commitTime();
      }
    }
  }

  // ---------- MODE SET ALARM ----------
  else if (mode == MODE_SET_ALARM) {

    if (pressed(3)) { // BTN_PLUS
      if (editField == 0) editYear++;
      else if (editField == 1) editMonth = (editMonth % 12) + 1;
      else if (editField == 2) editDay = (editDay % 31) + 1;
      else if (editField == 3) editHour = (editHour + 1) % 24;
      else if (editField == 4) editMinute = (editMinute + 1) % 60;
      showSetAlarmMenu();
    }

    if (pressed(4)) { // BTN_MINUS
      if (editField == 0) editYear--;
      else if (editField == 1) editMonth = (editMonth == 1 ? 12 : editMonth - 1);
      else if (editField == 2) editDay = (editDay == 1 ? 31 : editDay - 1);
      else if (editField == 3) editHour = (editHour == 0 ? 23 : editHour - 1);
      else if (editField == 4) editMinute = (editMinute == 0 ? 59 : editMinute - 1);
      showSetAlarmMenu();
    }

    if (pressed(2)) { // BTN_OK
      if (editField < 4) {
        editField++;
        showSetAlarmMenu();
      } else {
        commitAlarm();
      }
    }
  }

  // ---------- MODE ALARM RINGING ----------
  else if (mode == MODE_ALARM_RINGING) {

    if (pressed(5)) { // BTN_STOP
      stopAlarm();
      return;
    }

    // stop after 1 minute automatically
    if (millis() - alarmStartTime > 60000) {
      stopAlarm();
      return;
    }
  }
}
