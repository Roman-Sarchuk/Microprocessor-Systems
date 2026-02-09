#define F_CPU 10000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdint.h>

/* ----------- SHIFT REGISTER 74HC595 ----------- */
#define SH_DATA  PB4
#define SH_CLK   PB5
#define SH_LATCH PB6

/* ----------- DIGITS ----------- */
#define DIG1 PC0
#define DIG2 PC1
#define DIG3 PC2

/* ----------- RGB LED ----------- */
#define RED_PIN   PB3
#define GREEN_PIN PB2
#define BLUE_PIN  PB1

/* ----------- ADC ----------- */
#define LM35_CH 3   // PA3

/* ----------- GLOBAL VARIABLES ----------- */
volatile uint16_t temp10 = 0;
volatile uint8_t current_digit = 0;

/* ----------- DIGIT CODES (A-G,DP) ----------- */
const uint8_t digitCode[11] = {
	0x3F, // 0
	0x06, // 1
	0x5B, // 2
	0x4F, // 3
	0x66, // 4
	0x6D, // 5
	0x7D, // 6
	0x07, // 7
	0x7F, // 8
	0x6F, // 9
	0x00  // blank
};

/* ----------- SHIFT REGISTER SEND ----------- */
void shiftSend(uint8_t data)
{
	/* Latch LOW */
	PORTB &= ~(1 << SH_LATCH);

	/* Send 8 bits MSB first */
	for (int8_t i = 7; i >= 0; i--) {
		if (data & (1 << i)) PORTB |= (1 << SH_DATA);
		else PORTB &= ~(1 << SH_DATA);

		PORTB |=  (1 << SH_CLK);
		PORTB &= ~(1 << SH_CLK);
	}

	/* Latch HIGH */
	PORTB |=  (1 << SH_LATCH);
}

/* ----------- ADC ----------- */
void adc_init()
{
	ADMUX = (1 << REFS0) | (LM35_CH & 0x0F);
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
	_delay_ms(1);
}

uint16_t adc_read(uint8_t ch)
{
	ADMUX = (ADMUX & 0xF0) | (ch & 0x0F);
	_delay_us(10);
	ADCSRA |= (1 << ADSC);
	while (ADCSRA & (1 << ADSC));
	return ADC;
}

/* ----------- RGB ----------- */
void rgb_off()
{
	PORTB &= ~((1<<RED_PIN)|(1<<GREEN_PIN)|(1<<BLUE_PIN));
}

void rgb_update(float t)
{
	rgb_off();
	if (t >= 33.0) PORTB |= (1 << RED_PIN);
	else if (t >= 25.0) PORTB |= (1 << GREEN_PIN);
	else if (t > 18.0) PORTB |= (1 << BLUE_PIN);
}

/* ----------- TIMER1 ISR: MULTIPLEX ----------- */
ISR(TIMER1_OVF_vect)
{
	/* Turn off digits */
	PORTC &= ~((1<<DIG1)|(1<<DIG2)|(1<<DIG3));

	int digitValue = 10; // blank
	uint8_t dp = 0;

	switch (current_digit)
	{
		case 0: // hundreds
		digitValue = (temp10 / 100) % 10;
		dp = 0;
		if (temp10 >= 100) PORTC |= (1<<DIG1);
		break;

		case 1: // tens
		digitValue = (temp10 / 10) % 10;
		dp = 1;
		PORTC |= (1<<DIG2);
		break;

		case 2: // ones
		digitValue = temp10 % 10;
		dp = 0;
		PORTC |= (1<<DIG3);
		break;
	}

	uint8_t code = digitCode[digitValue];
	if (dp) code |= 0x80;

	shiftSend(code);

	current_digit++;
	if (current_digit > 2) current_digit = 0;

	TCNT1 = 0x0000;
}

/* ----------- MAIN ----------- */
int main()
{
	/* PORT SETUP */
	DDRB |= (1<<SH_DATA)|(1<<SH_CLK)|(1<<SH_LATCH);
	DDRB |= (1<<RED_PIN)|(1<<GREEN_PIN)|(1<<BLUE_PIN);

	DDRC |= (1<<DIG1)|(1<<DIG2)|(1<<DIG3);

	PORTC &= ~((1<<DIG1)|(1<<DIG2)|(1<<DIG3));
	rgb_off();

	adc_init();

	/* Timer1: fast refresh */
	TCCR1A = 0x00;
	TCCR1B = (1 << CS10);   // no prescaler
	TIMSK  = (1 << TOIE1);  // enable overflow interrupt
	sei();

	while (1)
	{
		uint16_t raw = adc_read(LM35_CH);
		float t = (raw * 500.0) / 1023.0;

		rgb_update(t);

		temp10 = (int)(t * 10 + 0.5);
		if (temp10 < 0) temp10 = 0;
		if (temp10 > 999) temp10 = 999;

		_delay_ms(300);
	}
}