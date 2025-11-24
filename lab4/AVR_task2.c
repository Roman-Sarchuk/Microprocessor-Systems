#define F_CPU 8000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

// --- Button pins on PORTA bits 1–4 ---
#define S1 PA1

// --- Segment pins on PORTD bits 0–6 ---
#define SegA PD0
#define SegB PD1
#define SegC PD2
#define SegD PD3
#define SegE PD4
#define SegF PD5
#define SegG PD6

// --- Digit select pins on PORTC bits 0–3 ---
#define Dig1 PC0
#define Dig2 PC1
#define Dig3 PC2
#define Dig4 PC3

// --- Variables ---
static unsigned long seed = 1;
volatile uint8_t current_digit = 1;
volatile uint16_t value = 0;

// --- Function prototypes ---
void disp_off(void);
void disp(uint8_t number);


int main(void)
{
	// --- Configure segments as outputs ---
	DDRD = 0x7F;  // PD0–PD6 outputs
	PORTD = 0x00; // All segments off

	// --- Configure digits as outputs ---
	DDRC = 0x0F;  // PC0–PC3 outputs
	PORTC = 0x00; // All digits off

	// --- Configure buttons as inputs with pull-ups ---
	DDRA &= ~(1 << S1);
	PORTA |= (1 << S1);	 // enable pull-ups
	
	// --- Timer1 setup ---
	TCCR1A = 0x00;
	TCCR1B = (1 << CS10);  // no prescaler
	TIMSK  = (1 << TOIE1); // enable overflow interrupt
	TCNT1  = 0x0000;

	sei(); // enable global interrupts

	// --- Main loop ---
	while (1)
	{
		if (!(PINA & (1 << S1))) {
			value = rand() % 10000;
			_delay_ms(200);
		}
	}
}


// --- Timer1 overflow interrupt for multiplexing ---
ISR(TIMER1_OVF_vect)
{
	disp_off(); // turn off all digits

	uint8_t digits[4];
	digits[0] = (value / 1000) % 10;
	digits[1] = (value / 100) % 10;
	digits[2] = (value / 10) % 10;
	digits[3] = value % 10;

	switch(current_digit)
	{
		case 1: disp(digits[0]); PORTC |= (1 << Dig1); break;
		case 2: disp(digits[1]); PORTC |= (1 << Dig2); break;
		case 3: disp(digits[2]); PORTC |= (1 << Dig3); break;
		case 4: disp(digits[3]); PORTC |= (1 << Dig4); break;
	}

	current_digit++;
	if(current_digit > 4) current_digit = 1;
}


// --- rand num ---
void srand(unsigned long s) {
	seed = s;
}

int rand(void) {
	seed = seed * 1103515245 + 12345;
	return (unsigned int)(seed >> 16);
}


// --- Turn off all digits ---
void disp_off(void)
{
	PORTC &= ~((1 << Dig1) | (1 << Dig2) | (1 << Dig3) | (1 << Dig4));
}


// --- Display one digit on 7-segment display ---
void disp(uint8_t number)
{
	switch (number)
	{
		case 0: PORTD = (1 << SegA) | (1 << SegB) | (1 << SegC) | (1 << SegD) | (1 << SegE) | (1 << SegF); break;
		case 1: PORTD = (1 << SegB) | (1 << SegC); break;
		case 2: PORTD = (1 << SegA) | (1 << SegB) | (1 << SegD) | (1 << SegE) | (1 << SegG); break;
		case 3: PORTD = (1 << SegA) | (1 << SegB) | (1 << SegC) | (1 << SegD) | (1 << SegG); break;
		case 4: PORTD = (1 << SegB) | (1 << SegC) | (1 << SegF) | (1 << SegG); break;
		case 5: PORTD = (1 << SegA) | (1 << SegC) | (1 << SegD) | (1 << SegF) | (1 << SegG); break;
		case 6: PORTD = (1 << SegA) | (1 << SegC) | (1 << SegD) | (1 << SegE) | (1 << SegF) | (1 << SegG); break;
		case 7: PORTD = (1 << SegA) | (1 << SegB) | (1 << SegC); break;
		case 8: PORTD = (1 << SegA) | (1 << SegB) | (1 << SegC) | (1 << SegD) | (1 << SegE) | (1 << SegF) | (1 << SegG); break;
		case 9: PORTD = (1 << SegA) | (1 << SegB) | (1 << SegC) | (1 << SegD) | (1 << SegF) | (1 << SegG); break;
	}
}
