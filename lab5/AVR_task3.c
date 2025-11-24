#define F_CPU 1000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <avr/interrupt.h>

#define CLOCK_PIN PD0
#define DATA_PIN  PD1
#define LATCH_PIN PD2

#define DIG1 PD3
#define DIG2 PD4
#define DIG3 PD5
#define DIG4 PD6

#define BTN1 PC0
#define BTN2 PC1
#define BTN3 PC2
#define BTN4 PC3

volatile uint8_t current_digit = 0;
volatile uint8_t digits[4] = {0, 0, 0, 0};

uint8_t btn_prev[4] = {1, 1, 1, 1};

const uint8_t numeral[10] = {
	0xFC, // 0
	0x60, // 1
	0xDA, // 2
	0xF2, // 3
	0x66, // 4
	0xB6, // 5
	0xBE, // 6
	0xE0, // 7
	0xFE, // 8
	0xF6  // 9
};

void shiftOut(uint8_t data);
void disp_off(void);
void showDigit(uint8_t digit_pos, uint8_t num);

int main(void) {
	DDRD |= (1<<CLOCK_PIN)|(1<<DATA_PIN)|(1<<LATCH_PIN)|(1<<DIG1)|(1<<DIG2)|(1<<DIG3)|(1<<DIG4);
	DDRC &= ~((1<<BTN1)|(1<<BTN2)|(1<<BTN3)|(1<<BTN4));
	PORTC |= (1<<BTN1)|(1<<BTN2)|(1<<BTN3)|(1<<BTN4);

	TCCR1A = 0;
	TCCR1B = (1<<CS10);
	TIMSK = (1<<TOIE1);
	TCNT1 = 0;
	sei();

	while(1) {
		uint8_t btn_state[4];
		btn_state[0] = (PINC & (1<<BTN1)) ? 1 : 0;
		btn_state[1] = (PINC & (1<<BTN2)) ? 1 : 0;
		btn_state[2] = (PINC & (1<<BTN3)) ? 1 : 0;
		btn_state[3] = (PINC & (1<<BTN4)) ? 1 : 0;

		for(uint8_t i=0; i<4; i++){
			if(btn_prev[i] && !btn_state[i]){
				_delay_ms(50);
				if(!(PINC & (1<< (BTN1+i)))){
					digits[i] = (digits[i]+1) % 10;
				}
			}
			btn_prev[i] = btn_state[i];
		}

		_delay_ms(10);
	}
}

ISR(TIMER1_OVF_vect){
	disp_off();
	showDigit(current_digit, digits[current_digit]);
	current_digit = (current_digit+1) % 4;
}

void disp_off(void){
	PORTD &= ~((1<<DIG1)|(1<<DIG2)|(1<<DIG3)|(1<<DIG4));
}

void showDigit(uint8_t digit_pos, uint8_t num){
	PORTD &= ~((1<<DIG1)|(1<<DIG2)|(1<<DIG3)|(1<<DIG4));
	PORTD &= ~(1<<LATCH_PIN);
	shiftOut(numeral[num]);
	PORTD |= (1<<LATCH_PIN);

	switch(digit_pos){
		case 0: PORTD |= (1<<DIG1); break;
		case 1: PORTD |= (1<<DIG2); break;
		case 2: PORTD |= (1<<DIG3); break;
		case 3: PORTD |= (1<<DIG4); break;
	}
}

void shiftOut(uint8_t data){
	PORTD &= ~(1<<LATCH_PIN);
	for(uint8_t i=0;i<8;i++){
		if(data & (1<<(7-i))) PORTD |= (1<<DATA_PIN);
		else PORTD &= ~(1<<DATA_PIN);
		PORTD |= (1<<CLOCK_PIN);
		PORTD &= ~(1<<CLOCK_PIN);
	}
	PORTD |= (1<<LATCH_PIN);
}