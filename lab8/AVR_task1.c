#define F_CPU 16000000UL
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

/* ---------------- DHT ----------------- */
#define DHT_PIN PB0

uint8_t DHT11_Read(float* temperature, float* humidity){
	uint8_t bits[5] = {0};
	uint8_t i,j;

	DDRB |= (1<<DHT_PIN);
	PORTB &= ~(1<<DHT_PIN);
	_delay_ms(20);
	PORTB |= (1<<DHT_PIN);
	_delay_us(40);
	DDRB &= ~(1<<DHT_PIN);

	_delay_us(80);
	if(!(PINB & (1<<DHT_PIN))) _delay_us(80);
	if(PINB & (1<<DHT_PIN)) _delay_us(80);

	for(i=0;i<5;i++){
		for(j=0;j<8;j++){
			while(!(PINB & (1<<DHT_PIN)));
			_delay_us(35);
			if(PINB & (1<<DHT_PIN))
			bits[i] |= (1<<(7-j));
			while(PINB & (1<<DHT_PIN));
		}
	}

	if(bits[4] != (bits[0]+bits[1]+bits[2]+bits[3])) return 0;

	*humidity = bits[0];
	*temperature = bits[2];
	return 1;
}


/* ------------- MAIN LOOP ------------ */
int main() {
	lcd_init();
	adc_init();
	
	DDRB |= (1<<RPIN)|(1<<GPIN)|(1<<BPIN);
	rgb_off();

	char buf[16];
	float t, h;
	
	while(1) {
		if(!DHT11_Read(&t,&h)){
			lcd_cmd(0x01);
			_delay_ms(2);
			lcd_cmd(0x80);
			lcd_print("Cannot read DHT");
			rgb_off();
			_delay_ms(1000);
			continue;
		}
		
		// ---- RGB ----
		if(t <= 18.0) rgb_off();
		else if(t < 25.0) rgb_set('B');
		else if(t < 33.0) rgb_set('G');
		else rgb_set('R');

		// ---- LCD ----
		lcd_cmd(0x01);
		_delay_ms(2);
		lcd_cmd(0x80);
		lcd_print("T:");
		dtostrf(t, 5, 1, buf);
		lcd_print(buf);
		lcd_data(0xDF); // gradus sign
		lcd_print("C");
		_delay_ms(2);
		
		lcd_cmd(0xC0);
		_delay_ms(2);
		lcd_print("H:");
		dtostrf(h, 5, 1, buf);
		lcd_print(buf);
		lcd_print("%");

		_delay_ms(1000);
	}
}