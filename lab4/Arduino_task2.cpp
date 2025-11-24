// --- Connecting button ---
#define button A1

// --- Segment pins ---
#define SegA  2
#define SegB  3
#define SegC  4
#define SegD  5
#define SegE  6
#define SegF  7
#define SegG  8

// --- Digit (position) pins ---
#define Dig1  9
#define Dig2  10
#define Dig3  11
#define Dig4  12

// --- Variables ---
byte current_digit = 1;
int count = 0;

// --- Initialization ---
void setup()
{
  pinMode(button, INPUT_PULLUP);

  pinMode(SegA, OUTPUT);
  pinMode(SegB, OUTPUT);
  pinMode(SegC, OUTPUT);
  pinMode(SegD, OUTPUT);
  pinMode(SegE, OUTPUT);
  pinMode(SegF, OUTPUT);
  pinMode(SegG, OUTPUT);

  pinMode(Dig1, OUTPUT);
  pinMode(Dig2, OUTPUT);
  pinMode(Dig3, OUTPUT);
  pinMode(Dig4, OUTPUT);

  disp_off();

  // --- Timer 1 configuration ---
  TCCR1A = 0;
  TCCR1B = 1;     // enable Timer1 with prescaler = 1
  TCNT1 = 0;      // initial timer value
  TIMSK1 = 1;     // enable interrupt on timer overflow
}


// --- Timer1 interrupt handler ---
ISR(TIMER1_OVF_vect)
{
  disp_off(); // спочатку вимикаємо всі цифри

  switch (current_digit)
  {
    case 1:
      disp(count / 1000);     // тисячі
      digitalWrite(Dig1, HIGH);
      break;

    case 2:
      disp((count / 100) % 10); // сотні
      digitalWrite(Dig2, HIGH);
      break;

    case 3:
      disp((count / 10) % 10);  // десятки
      digitalWrite(Dig3, HIGH);
      break;

    case 4:
      disp(count % 10);        // одиниці
      digitalWrite(Dig4, HIGH);
      break;
  }

  // переходимо до наступного розряду
  current_digit = (current_digit % 4) + 1;
}


// --- Main loop ---
void loop()
{
  if (digitalRead(button) == 0)
  {
    count = random(0, 10000);

    delay(200);      // антидребезг кнопки (200 мс)
  }
}


// --- Function to display one digit on the segments ---
void disp(byte number)
{
  switch (number)
  {
    case 0:
      digitalWrite(SegA, HIGH);
      digitalWrite(SegB, HIGH);
      digitalWrite(SegC, HIGH);
      digitalWrite(SegD, HIGH);
      digitalWrite(SegE, HIGH);
      digitalWrite(SegF, HIGH);
      digitalWrite(SegG, LOW);
      break;

    case 1:
      digitalWrite(SegA, LOW);
      digitalWrite(SegB, HIGH);
      digitalWrite(SegC, HIGH);
      digitalWrite(SegD, LOW);
      digitalWrite(SegE, LOW);
      digitalWrite(SegF, LOW);
      digitalWrite(SegG, LOW);
      break;

    case 2:
      digitalWrite(SegA, HIGH);
      digitalWrite(SegB, HIGH);
      digitalWrite(SegC, LOW);
      digitalWrite(SegD, HIGH);
      digitalWrite(SegE, HIGH);
      digitalWrite(SegF, LOW);
      digitalWrite(SegG, HIGH);
      break;

    case 3:
      digitalWrite(SegA, HIGH);
      digitalWrite(SegB, HIGH);
      digitalWrite(SegC, HIGH);
      digitalWrite(SegD, HIGH);
      digitalWrite(SegE, LOW);
      digitalWrite(SegF, LOW);
      digitalWrite(SegG, HIGH);
      break;

    case 4:
      digitalWrite(SegA, LOW);
      digitalWrite(SegB, HIGH);
      digitalWrite(SegC, HIGH);
      digitalWrite(SegD, LOW);
      digitalWrite(SegE, LOW);
      digitalWrite(SegF, HIGH);
      digitalWrite(SegG, HIGH);
      break;

    case 5:
      digitalWrite(SegA, HIGH);
      digitalWrite(SegB, LOW);
      digitalWrite(SegC, HIGH);
      digitalWrite(SegD, HIGH);
      digitalWrite(SegE, LOW);
      digitalWrite(SegF, HIGH);
      digitalWrite(SegG, HIGH);
      break;

    case 6:
      digitalWrite(SegA, HIGH);
      digitalWrite(SegB, LOW);
      digitalWrite(SegC, HIGH);
      digitalWrite(SegD, HIGH);
      digitalWrite(SegE, HIGH);
      digitalWrite(SegF, HIGH);
      digitalWrite(SegG, HIGH);
      break;

    case 7:
      digitalWrite(SegA, HIGH);
      digitalWrite(SegB, HIGH);
      digitalWrite(SegC, HIGH);
      digitalWrite(SegD, LOW);
      digitalWrite(SegE, LOW);
      digitalWrite(SegF, LOW);
      digitalWrite(SegG, LOW);
      break;

    case 8:
      digitalWrite(SegA, HIGH);
      digitalWrite(SegB, HIGH);
      digitalWrite(SegC, HIGH);
      digitalWrite(SegD, HIGH);
      digitalWrite(SegE, HIGH);
      digitalWrite(SegF, HIGH);
      digitalWrite(SegG, HIGH);
      break;

    case 9:
      digitalWrite(SegA, HIGH);
      digitalWrite(SegB, HIGH);
      digitalWrite(SegC, HIGH);
      digitalWrite(SegD, HIGH);
      digitalWrite(SegE, LOW);
      digitalWrite(SegF, HIGH);
      digitalWrite(SegG, HIGH);
      break;
  }
}

// --- Turn off all digits ---
void disp_off()
{
  digitalWrite(Dig1, LOW);
  digitalWrite(Dig2, LOW);
  digitalWrite(Dig3, LOW);
  digitalWrite(Dig4, LOW);
}
