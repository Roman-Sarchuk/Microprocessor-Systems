#define F_CPU 10000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>

/* ------ LCD PIN MAP ------ */
#define RS PD0
#define E  PD1
#define D4 PD4
#define D5 PD5
#define D6 PD6
#define D7 PD7

/* ------ BUTTON PIN ------ */
#define BTN_PIN PC0

/* ---------------- LCD LOW LEVEL ---------------- */

void lcd_pulse() {
	PORTD |= (1 << E);
	_delay_us(1);
	PORTD &= ~(1 << E);
	_delay_us(100);
}

void lcd_nibble(uint8_t nib) {
	(nib & 1) ? (PORTD |= 1 << D4) : (PORTD &= ~(1 << D4));
	(nib & 2) ? (PORTD |= 1 << D5) : (PORTD &= ~(1 << D5));
	(nib & 4) ? (PORTD |= 1 << D6) : (PORTD &= ~(1 << D6));
	(nib & 8) ? (PORTD |= 1 << D7) : (PORTD &= ~(1 << D7));
	lcd_pulse();
}

void lcd_byte(uint8_t data, uint8_t rs) {
	if (rs) PORTD |=  (1 << RS);
	else    PORTD &= ~(1 << RS);

	lcd_nibble(data >> 4);
	lcd_nibble(data & 0x0F);
}

void lcd_cmd(uint8_t cmd) {
	lcd_byte(cmd, 0);
}

void lcd_data(uint8_t d) {
	lcd_byte(d, 1);
}

void lcd_clear() {
	lcd_cmd(0x01);
	_delay_ms(2);
}

void lcd_print(const char *text) {
	for (uint8_t i = 0; text[i] != '\0'; i++) {
		lcd_data(text[i]);
	}
}

void lcd_init() {
	DDRD |= (1<<RS)|(1<<E)|(1<<D4)|(1<<D5)|(1<<D6)|(1<<D7);

	_delay_ms(50);

	lcd_nibble(0x03);
	_delay_ms(5);
	lcd_nibble(0x03);
	_delay_us(150);
	lcd_nibble(0x03);
	lcd_nibble(0x02);

	lcd_cmd(0x28);  // 4-bit, 2 line
	lcd_cmd(0x0C);  // display ON
	lcd_cmd(0x06);  // cursor increment
	lcd_clear();
}

void lcd_set_cursor(uint8_t col, uint8_t row) {
	lcd_cmd(0x80 | (row ? (0x40 + col) : col));
}

/* --------- RANDOM --------- */
uint32_t seed = 12345;

uint32_t rand32() {
	seed = seed * 1103515245 + 12345;
	return (seed >> 1);
}

/* ---------------- PRINT NUMBER ---------------- */

void printRandNum() {
	lcd_clear();
	lcd_set_cursor(0, 0);

	uint32_t n = rand32();

	char buf[11];
	utoa(n, buf, 10);
	
	_delay_ms(10);
	lcd_print(buf);
}

/* ---------------- MAIN ---------------- */

int main(void) {
	lcd_init();

	DDRC &= ~(1 << BTN_PIN);
	PORTC |= (1 << BTN_PIN);

	uint8_t lastState = 1;
	uint8_t state;

	printRandNum();

	while (1) {
		state = PINC & (1<<BTN_PIN);

		if (lastState && !state) {
			printRandNum();
			_delay_ms(200);
		}

		lastState = state;
	}
}
