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
const int len = sizeof(fullName) - 1;
const int width = 16;

volatile int pos = 0;

/* ---------------- LCD LOW LEVEL ---------------- */

void lcd_pulse_enable() {
	PORTD |= (1 << E);
	_delay_us(1);
	PORTD &= ~(1 << E);
	_delay_us(100);
}

void lcd_send_nibble(uint8_t nibble) {
	if (nibble & 0x01) PORTD |=  (1 << D4); else PORTD &= ~(1 << D4);
	if (nibble & 0x02) PORTD |=  (1 << D5); else PORTD &= ~(1 << D5);
	if (nibble & 0x04) PORTD |=  (1 << D6); else PORTD &= ~(1 << D6);
	if (nibble & 0x08) PORTD |=  (1 << D7); else PORTD &= ~(1 << D7);

	lcd_pulse_enable();
}

void lcd_send_byte(uint8_t byte, uint8_t isData) {
	if (isData) PORTD |=  (1 << RS); else PORTD &= ~(1 << RS);

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

	lcd_cmd(0x28); // 4-bit, 2 lines
	lcd_cmd(0x0C); // display on
	lcd_cmd(0x06); // increment cursor
	lcd_cmd(0x01); // clear
	_delay_ms(2);
}

void lcd_set_cursor(uint8_t col, uint8_t row) {
	uint8_t addr = (row == 0) ? col : col + 0x40;
	lcd_cmd(0x80 | addr);
}

/* ---------------- TIMER 1 ISR ---------------- */

ISR(TIMER1_COMPA_vect) {
	lcd_set_cursor(0, 0);

	for (int i = 0; i < width; i++) {
		int index = (pos + i) % len;
		lcd_data(fullName[index]);
	}

	pos++;
	if (pos >= len) pos = 0;
}

/* ---------------- MAIN ---------------- */

int main(void) {
	lcd_init();

	/* Timer1 CTC */
	TCCR1A = 0;
	TCCR1B = 0;
	TCNT1 = 0;

	// OCR1A = 6249 corresponds to 0.5s for 16MHz and prescaler 1024
	OCR1A = 6249;

	TCCR1B |= (1 << WGM12);                 // CTC mode
	TCCR1B |= (1 << CS12) | (1 << CS10);    // prescaler 1024
	TIMSK  |= (1 << OCIE1A);                // enable interrupt (ATmega32 uses TIMSK)

	sei();

	while (1) {
	}
}
