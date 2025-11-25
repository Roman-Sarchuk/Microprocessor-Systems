#define F_CPU 10000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define RS PD0
#define E  PD1
#define D4 PD4
#define D5 PD5
#define D6 PD6
#define D7 PD7

const char fullName[] = "Sarchuk Roman Andriovych ";
const uint8_t len = sizeof(fullName) - 1;
const uint8_t width = 16;

const char date[] = "18.11.2025";
const char time[] = "11:40     ";

volatile uint8_t pos = 0;

/* ---------------- LCD LOW LEVEL ---------------- */

void lcd_pulse_enable() {
	PORTD |= (1 << E);
	_delay_us(1);
	PORTD &= ~(1 << E);
	_delay_us(100);
}

void lcd_send_nibble(uint8_t nibble) {
	if (nibble & 1) PORTD |= (1 << D4); else PORTD &= ~(1 << D4);
	if (nibble & 2) PORTD |= (1 << D5); else PORTD &= ~(1 << D5);
	if (nibble & 4) PORTD |= (1 << D6); else PORTD &= ~(1 << D6);
	if (nibble & 8) PORTD |= (1 << D7); else PORTD &= ~(1 << D7);

	lcd_pulse_enable();
}

void lcd_send_byte(uint8_t byte, uint8_t isData) {
	if (isData) PORTD |= (1 << RS);
	else        PORTD &= ~(1 << RS);

	lcd_send_nibble(byte >> 4);
	lcd_send_nibble(byte & 0x0F);
}

void lcd_cmd(uint8_t cmd) {
	lcd_send_byte(cmd, 0);
}

void lcd_data(uint8_t data) {
	lcd_send_byte(data, 1);
}

void lcd_init() {
	DDRD |= (1 << RS) | (1 << E) | (1 << D4) | (1 << D5) | (1 << D6) | (1 << D7);

	_delay_ms(50);

	lcd_send_nibble(0x03);
	_delay_ms(5);
	lcd_send_nibble(0x03);
	_delay_us(150);
	lcd_send_nibble(0x03);
	lcd_send_nibble(0x02);

	lcd_cmd(0x28);
	lcd_cmd(0x0C);
	lcd_cmd(0x06);
	lcd_cmd(0x01);
	_delay_ms(2);
}

void lcd_set_cursor(uint8_t col, uint8_t row) {
	uint8_t addr = (row == 0) ? col : col + 0x40;
	lcd_cmd(0x80 | addr);
}

void lcd_print(const char *text) {
	for (uint8_t i = 0; text[i] != '\0'; i++) {
		lcd_data(text[i]);
	}
}

/* ---------------- TIMER 1 ISR ---------------- */

ISR(TIMER1_COMPA_vect) {
	lcd_set_cursor(0, 0);

	uint8_t index;
	for (uint8_t i = 0; i < width; i++) {
		index = (pos + i) % len;
		lcd_data(fullName[index]);
	}

	pos++;
	if (pos >= len) pos = 0;
}

/* ---------------- MAIN ---------------- */

int main(void) {
	lcd_init();

	// Timer1 CTC
	TCCR1A = 0;
	TCCR1B = 0;
	TCNT1 = 0;

	OCR1A = 6249; // ~0.5 s
	TCCR1B |= (1 << WGM12);
	TCCR1B |= (1 << CS12) | (1 << CS10);
	TIMSK |= (1 << OCIE1A);

	sei();

	while (1) {
		lcd_set_cursor(0, 1);
		lcd_print("Date: ");
		lcd_print(date);
		_delay_ms(1000);

		lcd_set_cursor(0, 1);
		lcd_print("Time: ");
		lcd_print(time);
		_delay_ms(1000);
	}
}
