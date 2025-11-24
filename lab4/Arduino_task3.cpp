// --- Connecting buttons ---
#define S1 A1
#define S2 A2
#define S3 A3
#define S4 A4

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
int digits[4] = {0, 0, 0, 0};


// --- Initialization ---
void setup()
{
  pinMode(S1, INPUT_PULLUP);
  pinMode(S2, INPUT_PULLUP);
  pinMode(S3, INPUT_PULLUP);
  pinMode(S4, INPUT_PULLUP);

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
  disp_off(); // turn off all digits before switching

  switch (current_digit)
  {
    case 1:
      disp(digits[0]);
      digitalWrite(Dig1, HIGH);
      break;
    case 2:
      disp(digits[1]);
      digitalWrite(Dig2, HIGH);
      break;
    case 3:
      disp(digits[2]);
      digitalWrite(Dig3, HIGH);
      break;
    case 4:
      disp(digits[3]);
      digitalWrite(Dig4, HIGH);
      break;
  }

  current_digit = (current_digit % 4) + 1;
}


// --- Main loop ---
void loop()
{
  if (digitalRead(S1) == LOW)
  {
    digits[0] = (digits[0] + 1) % 10;
    delay(200);
  }
  if (digitalRead(S2) == LOW)
  {
    digits[1] = (digits[1] + 1) % 10;
    delay(200);
  }
  if (digitalRead(S3) == LOW)
  {
    digits[2] = (digits[2] + 1) % 10;
    delay(200);
  }
  if (digitalRead(S4) == LOW)
  {
    digits[3] = (digits[3] + 1) % 10;
    delay(200);
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
