#define F_CPU 10000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

/* ------ LCD PIN MAP ------ */
#define RS PB0
#define E  PB1
#define D4 PD4
#define D5 PD5
#define D6 PD6
#define D7 PD7

/* ------ BUTTONS ------ */
#define BTN_PLAY  PD2   // INT0
#define BTN_STOP  PD3   // INT1

volatile uint8_t btnPlayPressed = 0;
volatile uint8_t btnStopPressed = 0;

/* ---------------- LCD LOW LEVEL ---------------- */
void lcd_pulse() {
	PORTB |= (1 << E);
	_delay_us(1);
	PORTB &= ~(1 << E);
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
	if (rs) PORTB |=  (1 << RS);
	else    PORTB &= ~(1 << RS);

	lcd_nibble(data >> 4);
	lcd_nibble(data & 0x0F);
}

void lcd_cmd(uint8_t cmd) { lcd_byte(cmd, 0); }
void lcd_data(uint8_t d) { lcd_byte(d, 1); }

void lcd_clear() {
	lcd_cmd(0x01);
	_delay_ms(2);
}

void lcd_print(const char *s) {
	while (*s) lcd_data(*s++);
}

void lcd_set_cursor(uint8_t col, uint8_t row) {
	lcd_cmd(0x80 | (row ? (0x40 + col) : col));
}

void lcd_init() {
	DDRB |= (1<<RS)|(1<<E);
	PORTB &= ~((1<<RS)|(1<<E));

	DDRD |= (1<<D4)|(1<<D5)|(1<<D6)|(1<<D7);
	PORTD &= ~((1<<D4)|(1<<D5)|(1<<D6)|(1<<D7));

	_delay_ms(50);

	PORTB &= ~((1<<RS)|(1<<E));  // RS=0, E=0
	lcd_nibble(0x03);
	_delay_ms(5);
	lcd_nibble(0x03);
	_delay_us(150);
	lcd_nibble(0x03);
	lcd_nibble(0x02);

	lcd_cmd(0x28);  // 4-bit, 2-line
	lcd_cmd(0x0C);  // display ON
	lcd_cmd(0x06);  // cursor increment
	lcd_clear();
}

/* ---------------- INTERRUPTS ---------------- */
ISR(INT0_vect) { btnPlayPressed = 1; }
ISR(INT1_vect) { btnStopPressed = 1; }

/* ---------------- MAIN ---------------- */
int main(void) {
	lcd_init();

	/* Кнопки як входи з pull-up */
	DDRD &= ~((1 << BTN_PLAY) | (1 << BTN_STOP));
	PORTD |=  (1 << BTN_PLAY) | (1 << BTN_STOP);

	/* INT0 + INT1 FALLING edge */
	MCUCR = (1<<ISC01) | (1<<ISC11);

	/* enable INT0 & INT1 */
	GICR |= (1 << INT0) | (1 << INT1);

	sei();
	
	lcd_clear();
	lcd_set_cursor(0, 0);
	_delay_ms(10);
	lcd_print("LOAD");

	while (1) {
		if (btnPlayPressed) {
			lcd_clear();
			lcd_set_cursor(0, 0);
			_delay_ms(10);
			lcd_print("PLAY");
			btnPlayPressed = 0;
		}

		if (btnStopPressed) {
			lcd_clear();
			lcd_set_cursor(0, 0);
			_delay_ms(10);
			lcd_print("STOP");
			btnStopPressed = 0;
		}

		_delay_ms(50);  // простий антидребезг
	}
}
