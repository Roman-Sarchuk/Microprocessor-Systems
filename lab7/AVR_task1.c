#define F_CPU 10000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>

/* ---------------- LCD ---------------- */
#define RS PA0
#define E  PA2
#define D4 PA4
#define D5 PA5
#define D6 PA6
#define D7 PA7

#define LCD_DATA_MASK ((1<<D4)|(1<<D5)|(1<<D6)|(1<<D7))

void lcd_cmd(unsigned char cmd) {
	PORTA &= ~(1 << RS); // RS=0

	PORTA = (PORTA & ~LCD_DATA_MASK) | ((cmd >> 4) << 4);
	PORTA |= (1 << E);
	PORTA &= ~(1 << E);

	PORTA = (PORTA & ~LCD_DATA_MASK) | ((cmd & 0x0F) << 4);
	PORTA |= (1 << E);
	PORTA &= ~(1 << E);

	_delay_ms(2);
}

void lcd_data(unsigned char data) {
	PORTA |= (1 << RS); // RS=1

	PORTA = (PORTA & ~LCD_DATA_MASK) | ((data >> 4) << 4);
	PORTA |= (1 << E);
	PORTA &= ~(1 << E);

	PORTA = (PORTA & ~LCD_DATA_MASK) | ((data & 0x0F) << 4);
	PORTA |= (1 << E);
	PORTA &= ~(1 << E);

	_delay_ms(2);
}

void lcd_init() {
	DDRA |= (1<<RS)|(1<<E)|(1<<D4)|(1<<D5)|(1<<D6)|(1<<D7);
	_delay_ms(20);

	lcd_cmd(0x02); // 4-bit mode
	lcd_cmd(0x28); // 2 lines, 5x8
	lcd_cmd(0x0C); // Display on, cursor off
	lcd_cmd(0x06); // Increment cursor
	lcd_cmd(0x01); // Clear display
	_delay_ms(2);
}

void lcd_print(char *str) {
	while (*str) lcd_data(*str++);
}

/* -------- ADC INIT ----------------- */
#define LM35_ADC_CHANNEL 3

void adc_init() {
	ADMUX = (1 << REFS0) | (LM35_ADC_CHANNEL & 0x0F); // AVCC ref
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

/* -------- RGB ----------------- */
#define RPIN PB3
#define GPIN PB2
#define BPIN PB1

void rgb_off() {
	PORTB &= ~((1<<RPIN)|(1<<GPIN)|(1<<BPIN));
}

void rgb_set(char color) {
	rgb_off();
	switch(color) {
		case 'R': PORTB |= (1<<RPIN); break;
		case 'G': PORTB |= (1<<GPIN); break;
		case 'B': PORTB |= (1<<BPIN); break;
		default: break;
	}
}

/* ------------- MAIN LOOP ------------ */
int main() {
	lcd_init();
	adc_init();
	
	DDRB |= (1<<RPIN)|(1<<GPIN)|(1<<BPIN);
	rgb_off();

	char buf[16];
	float t;

	while(1) {
		uint16_t val = adc_read(LM35_ADC_CHANNEL);
		
		t = (val * 500.0) / 1023.0;
		
		// ---- RGB ----
		if(t <= 18.0) rgb_off();
		else if(t < 25.0) rgb_set('B');
		else if(t < 33.0) rgb_set('G');
		else rgb_set('R');

		// ---- LCD ----
		lcd_cmd(0x01);
		_delay_ms(2);
		lcd_cmd(0x80);
		lcd_print("Temperature:");
		lcd_cmd(0xC0);
		dtostrf(t, 5, 1, buf);
		lcd_print(buf);
		lcd_data(0xDF); // gradus sign
		lcd_print("C");

		_delay_ms(1000);
	}
}