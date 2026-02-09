#define F_CPU 10000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

/* ---------------- SEGMENTS & DIGITS ---------------- */
#define SEG_PORT PORTD
#define DIG_PORT PORTC
#define SEG_DDR  DDRD
#define DIG_DDR  DDRC

#define DIG1 PC0
#define DIG2 PC1
#define DIG3 PC2

/* ---------------- RGB ---------------- */
#define RED_PIN   PB0
#define GREEN_PIN PB1
#define BLUE_PIN  PB2

/* ---------------- ADC ---------------- */
#define LM35_ADC_CHANNEL 3  // PA3

/* ---------------- DIGIT CODES ---------------- */
const uint8_t digitCode[10] = {
	0x3F, // 0
	0x06, // 1
	0x5B, // 2
	0x4F, // 3
	0x66, // 4
	0x6D, // 5
	0x7D, // 6
	0x07, // 7
	0x7F, // 8
	0x6F  // 9
};

/* ---------------- ADC FUNCTIONS ---------------- */
void adc_init() {
	ADMUX = (1 << REFS0) | (LM35_ADC_CHANNEL & 0x0F); // AVCC reference
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // prescaler 128
	_delay_ms(1);
}

uint16_t adc_read(uint8_t ch) {
	ADMUX = (ADMUX & 0xF0) | (ch & 0x0F);
	_delay_us(10);
	ADCSRA |= (1 << ADSC);
	while (ADCSRA & (1 << ADSC));
	return ADC;
}

/* ---------------- RGB FUNCTIONS ---------------- */
void rgb_off() {
	PORTB &= ~((1<<RED_PIN)|(1<<GREEN_PIN)|(1<<BLUE_PIN));
}

void rgb_set(float t) {
	rgb_off();
	if(t >= 33.0) PORTB |= (1<<RED_PIN);
	else if(t >= 25.0) PORTB |= (1<<GREEN_PIN);
	else if(t > 18.0) PORTB |= (1<<BLUE_PIN);
}

/* ---------------- SEGMENT FUNCTIONS ---------------- */
void showDigitRaw(uint8_t code) {
	SEG_PORT = code; // put code directly to PD0-PD7
}

void displayOne(uint8_t pos, int8_t value, uint8_t dp) {
	// Turn off all digits
	DIG_PORT &= ~((1<<DIG1)|(1<<DIG2)|(1<<DIG3));

	if(value >= 0 && value <= 9) {
		uint8_t code = digitCode[value];
		if(dp) code |= 0x80; // add decimal point
		showDigitRaw(code);
		// Turn on selected digit
		switch(pos) {
			case 0: DIG_PORT |= (1<<DIG1); break;
			case 1: DIG_PORT |= (1<<DIG2); break;
			case 2: DIG_PORT |= (1<<DIG3); break;
		}
		} else {
		showDigitRaw(0x00); // blank
	}
}

/* ---------------- MAIN ---------------- */
int main() {
	// ---- INIT PORTS ----
	SEG_DDR = 0xFF;    // PD0-PD7 output
	DIG_DDR |= (1<<DIG1)|(1<<DIG2)|(1<<DIG3);
	DDRB |= (1<<RED_PIN)|(1<<GREEN_PIN)|(1<<BLUE_PIN);

	SEG_PORT = 0x00;
	DIG_PORT &= ~((1<<DIG1)|(1<<DIG2)|(1<<DIG3));
	rgb_off();

	adc_init();

	while(1) {
		uint16_t val = adc_read(LM35_ADC_CHANNEL);
		float t = (val * 500.0) / 1023.0; // LM35 temperature in Celsius

		rgb_set(t);

		int temp10 = (int)(t*10 + 0.5); // one decimal

		int d2 = (temp10 / 100) % 10;
		int d1 = (temp10 / 10) % 10;
		int d0 = temp10 % 10;

		uint8_t leftBlank = (temp10 < 100);

		// multiplex 200ms
		for(uint16_t t0=0; t0<200; t0+=5) {
			if(leftBlank) displayOne(0, -1, 0);
			else displayOne(0, d2, 0);
			_delay_ms(2);

			displayOne(1, d1, 1); // decimal point
			_delay_ms(2);

			displayOne(2, d0, 0);
			_delay_ms(2);
		}

		_delay_ms(50);
	}
}
