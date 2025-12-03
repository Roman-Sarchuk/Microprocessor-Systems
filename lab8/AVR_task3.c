#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdint.h>

/* ---------- PIN MAPPINGS (as provided) ---------- */
#define SH_LATCH PC0
#define SH_CLK   PC1
#define SH_DATA  PC2

#define DIG1 PC3
#define DIG2 PC4
#define DIG3 PC5
#define DIG4 PC6

#define DHT_PIN PB0
#define B_PIN   PB1
#define G_PIN   PB2
#define R_PIN   PB3

/* ---------- Segment codes (a b c d e f g dp) ---------- */
const uint8_t digitCode[10] = {
	0b00111111, //0
	0b00000110, //1
	0b01011011, //2
	0b01001111, //3
	0b01100110, //4
	0b01101101, //5
	0b01111101, //6
	0b00000111, //7
	0b01111111, //8
	0b01101111  //9
};

/* ---------- GLOBALS ---------- */
volatile uint8_t current_digit = 0;
volatile uint8_t t1 = 0, t2 = 0, h1 = 0, h2 = 0; 

/* ---------- 74HC595 ---------- */
void shiftSend(uint8_t data)
{
	PORTC &= ~(1 << SH_LATCH);

	for (int8_t i = 7; i >= 0; i--) {
		if (data & (1 << i)) PORTC |=  (1 << SH_DATA);
		else                 PORTC &= ~(1 << SH_DATA);

		PORTC |=  (1 << SH_CLK);
		PORTC &= ~(1 << SH_CLK);
	}

	PORTC |= (1 << SH_LATCH);
}

/* ---------- Turn off all digits ---------- */
static inline void digits_off(void) {
	PORTC &= ~((1<<DIG1)|(1<<DIG2)|(1<<DIG3)|(1<<DIG4));
}

/* ---------- DHT11 read ----------*/
uint8_t DHT11_Read(float* temperature, float* humidity) {
	uint8_t bits[5] = {0,0,0,0,0};
	uint8_t i, j;

	DDRB |= (1 << DHT_PIN);
	PORTB &= ~(1 << DHT_PIN);
	_delay_ms(20);

	PORTB |= (1 << DHT_PIN);
	_delay_us(40);
	DDRB &= ~(1 << DHT_PIN);

	_delay_us(80);
	if (!(PINB & (1 << DHT_PIN))) _delay_us(80);
	if (PINB & (1 << DHT_PIN)) _delay_us(80);

	for (i = 0; i < 5; i++) {
		for (j = 0; j < 8; j++) {
			uint16_t wait_cnt = 0;
			while (!(PINB & (1 << DHT_PIN))) {
				if (++wait_cnt > 10000) return 0;
			}
			_delay_us(35);
			if (PINB & (1 << DHT_PIN)) {
				bits[i] |= (1 << (7 - j));
			}
			wait_cnt = 0;
			while (PINB & (1 << DHT_PIN)) {
				if (++wait_cnt > 10000) return 0;
			}
		}
	}

	if (bits[4] != (uint8_t)(bits[0] + bits[1] + bits[2] + bits[3])) return 0;

	*humidity = bits[0];
	*temperature = bits[2];
	return 1;
}

/* ---------- RGB set ---------- */
void setRGB(uint8_t r, uint8_t g, uint8_t b) {
	if (r) PORTB |=  (1 << R_PIN); else PORTB &= ~(1 << R_PIN);
	if (g) PORTB |=  (1 << G_PIN); else PORTB &= ~(1 << G_PIN);
	if (b) PORTB |=  (1 << B_PIN); else PORTB &= ~(1 << B_PIN);
}

/* ---------- TIMER1 OVF ISR ---------- */
ISR(TIMER1_OVF_vect)
{
	digits_off();

	uint8_t value = 0x00;
	uint8_t dp = 0;

	switch (current_digit) {
		case 0:
		value = digitCode[t1];
		dp = 0;
		PORTC |= (1 << DIG1);
		break;
		case 1:
		value = digitCode[t2];
		dp = 1;
		PORTC |= (1 << DIG2);
		break;
		case 2:
		value = digitCode[h1];
		dp = 0;
		PORTC |= (1 << DIG3);
		break;
		case 3:
		value = digitCode[h2];
		dp = 0;
		PORTC |= (1 << DIG4);
		break;
	}

	if (dp) value |= 0x80;

	shiftSend(value);

	current_digit++;
	if (current_digit > 3) current_digit = 0;

	TCNT1 = 0x0000;
}

/* ---------- Initialization ---------- */
void init_ports_timer(void) {
	DDRC |= (1<<SH_LATCH)|(1<<SH_CLK)|(1<<SH_DATA)|(1<<DIG1)|(1<<DIG2)|(1<<DIG3)|(1<<DIG4);
	PORTC &= ~((1<<SH_LATCH)|(1<<SH_CLK)|(1<<SH_DATA)|(1<<DIG1)|(1<<DIG2)|(1<<DIG3)|(1<<DIG4));

	DDRB |= (1<<R_PIN)|(1<<G_PIN)|(1<<B_PIN);
	PORTB &= ~((1<<R_PIN)|(1<<G_PIN)|(1<<B_PIN));
}

/* ---------- MAIN ---------- */
int main(void)
{
	init_ports_timer();

	TCCR1A = 0x00;
	TCCR1B = (1 << CS10);
	TIMSK = (1 << TOIE1);
	sei();

	float temperature = 0.0f, humidity = 0.0f;
	uint16_t counter = 0;

	t1 = t2 = h1 = h2 = 0;

	while (1) {
		_delay_ms(200);
		counter++;
		if (counter >= 5) { // ~1 second
			counter = 0;
			if (DHT11_Read(&temperature, &humidity)) {
				int T = (int)temperature;
				int H = (int)humidity;
				if (T < 0) T = 0;
				if (T > 99) T = 99;
				if (H < 0) H = 0;
				if (H > 99) H = 99;

				t1 = T / 10;
				t2 = T % 10;
				h1 = H / 10;
				h2 = H % 10;
			}
			if (humidity < 40.0f) setRGB(0,0,1);
			else if (humidity <= 60.0f) setRGB(0,1,0);
			else setRGB(1,0,0);
		}
	}

	return 0;
}
