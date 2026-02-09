#define F_CPU 1000000UL
#include <avr/io.h>
#include <util/delay.h>

#define RLED PB3  // OC0
#define GLED PD5  // OC1A
#define BLED PD7  // OC2
#define POT 0


// --- ADC ---
void ADC_init(void) {
	ADMUX = (1 << REFS0); // AVCC reference
	ADCSRA = (1 << ADEN) | (1 << ADPS1) | (1 << ADPS0); // enable, prescaler=8
}

uint16_t ADC_read(uint8_t channel) {
	ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);
	ADCSRA |= (1 << ADSC);
	while (ADCSRA & (1 << ADSC));
	return ADC;
}

uint8_t map10to8(uint16_t val) {
	return (val * 255) / 1023;
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
	TCCR2 = (1 << WGM20) | (1 << WGM21) | (1 << COM21) | (1 << CS22);
}


// MAIN
int main(void) {
	ADC_init();
	PWM_init();

	while (1) {
		uint16_t val = ADC_read(POT);
		uint8_t red = map10to8(val);
		uint8_t green = 255 - red;
		uint8_t blue = red / 4;

		OCR0 = red;    // PB3
		OCR1A = green; // PD5
		OCR2 = blue;   // PD7

		_delay_ms(10);
	}
}
