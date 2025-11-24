#define F_CPU 100000UL
#include <avr/io.h>
#include <util/delay.h>

#define BLED PB1
#define GLED PB2
#define RLED PB3
#define POT PA0


// --- ADC ---
void ADC_init(void) {
	ADMUX = (1 << REFS0); // AVCC reference
	ADCSRA = (1 << ADEN) | (1 << ADPS1) | (1 << ADPS0); // enable, prescaler=8
}

uint16_t ADC_read(uint8_t channel) {
	ADMUX = (1 << REFS0) | (channel & 0x0F);
	ADCSRA |= (1 << ADSC);
	while (ADCSRA & (1 << ADSC));
	return ADC;
}

uint8_t map10to8(uint16_t val) {
	return (val * 8) / 1023;
}


// --- PWM ---
void PWM_init(void) {
	DDRB |= (1 << RLED);
	DDRD |= (1 << GLED) | (1 << BLED);

	// Timer0 – Fast PWM, non-inverting (OC0)
	TCCR0 = (1 << WGM00) | (1 << WGM01) | (1 << COM01) | (1 << CS01);

	// Timer1 – Fast PWM 8-bit, non-inverting (OC1A)
	TCCR1A = (1 << WGM10) | (1 << COM1A1);
	TCCR1B = (1 << WGM12) | (1 << CS11);

	// Timer2 – Fast PWM, non-inverting (OC2)
	TCCR2 = (1 << WGM20) | (1 << WGM21) | (1 << COM21) | (1 << CS21);
}

void setMode(uint16_t val) {
	uint8_t mask = (1 << RLED) | (1 << GLED) | (1 << BLED);
	PORTB &= ~mask;
	
	uint8_t mode = map10to8(val);

	switch (mode) {
		case 1: PORTB |= (1 << RLED); break;		// Red
		case 2: PORTB |= (1 << GLED); break;		// Green
		case 3: PORTB |= (1 << BLED); break;		// Blue
		case 4: PORTB |= (1 << RLED) | (1 << BLED); break;		// Purple (Red + Blue)
		case 5: PORTB |= (1 << GLED) | (1 << BLED); break;		// Turquoise (Blue + Green)
		case 6: PORTB |= (1 << RLED) | (1 << GLED); break;		// Orange (Red + Green)
		case 7: PORTB |= (1 << RLED) | (1 << GLED) | (1 << BLED); break;		// White (Red + Green + Blue)
		default: break;		// 0 or others
	}
}


// MAIN
int main(void) {
	ADC_init();
	
	// set up ports
	DDRB |= (1 << RLED) | (1 << GLED) | (1 << BLED);
	PORTB = 0x00;

	while (1) {
		uint16_t val = ADC_read(POT);
		
		setMode(val);

		_delay_ms(10);
	}
}
