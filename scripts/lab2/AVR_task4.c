#define F_CPU 1000000UL
#include <avr/io.h>
#include <util/delay.h>

#define HL1 PB3
#define HL2 PD5
#define HL3 PD4
#define HL4 PD7

void PWM_init()
{
	// TIMER0: Fast PWM, non-inverting 
	TCCR0 = (1 << WGM00) | (1 << WGM01) |	// Fast PWM
	(1 << COM01) |							// Non-inverting mode
	(1 << CS01);							// Prescaler = 8

	// TIMER1: Fast PWM 8-bit, non-inverting 
	TCCR1A = (1 << WGM10) |					// Fast PWM 8-bit (WGM10+WGM12)
	(1 << COM1A1) | (1 << COM1B1);			// Non-inverting for A,B
	TCCR1B = (1 << WGM12) | (1 << CS11);    // Prescaler = 8

	// TIMER2: Fast PWM, non-inverting 
	TCCR2 = (1 << WGM20) | (1 << WGM21) |	// Fast PWM
	(1 << COM21) |							// Non-inverting mode
	(1 << CS21);							// Prescaler = 8
}


// MAIN
int main(void) {
	// Set up ports as output
	DDRB |= (1 << HL1);
	DDRD |= (1 << HL2) | (1 << HL3) | (1 << HL4);
	
	PWM_init();
	
	// Set up brightness
	OCR0  = 70;
	OCR1A = 130;
	OCR1B = 190;  
	OCR2  = 255;  
	
	while (1) {
		
	}
}
