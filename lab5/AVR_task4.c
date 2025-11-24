#define F_CPU 1000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <avr/interrupt.h>

#define DATA_PIN  PD1
#define CLOCK_PIN PD0
#define LATCH_PIN PD2

#define DIG1 PD3
#define DIG2 PD4
#define DIG3 PD5
#define DIG4 PD6

#define S1 PC0
#define S2 PC1

volatile uint8_t current_digit = 0;
volatile uint8_t display[4]; 

const uint8_t symb[] = {
	0x1C, // L
	0xFC, // O
	0xEE, // A
	0xFC, // D
	0xCE, // P
	0x76, // Y
	0xB6, // S
	0x1E  // T
};

void shiftOut(uint8_t data);
void disp_off(void);
void showDigit(uint8_t digit_pos, uint8_t val);
void loadLOAD(void);
void loadPLAY(void);
void loadSTOP(void);

int main(void){
	DDRD |= (1<<DATA_PIN)|(1<<CLOCK_PIN)|(1<<LATCH_PIN)|(1<<DIG1)|(1<<DIG2)|(1<<DIG3)|(1<<DIG4);
	DDRC &= ~((1<<S1)|(1<<S2));
	PORTC |= (1<<S1)|(1<<S2);

	TCCR1A = 0;
	TCCR1B = (1<<CS10);
	TIMSK = (1<<TOIE1);
	TCNT1 = 0;
	sei();

    loadLOAD();

	while(1){
		if(!(PINC & (1<<S1))){
			loadPLAY();
			_delay_ms(300); 
		}
		if(!(PINC & (1<<S2))){
			loadSTOP();
			_delay_ms(300);
		}
		_delay_ms(10);
	}
}

ISR(TIMER1_OVF_vect){
	disp_off();
	showDigit(current_digit, display[current_digit]);
	current_digit = (current_digit + 1) % 4;
}

void disp_off(void){
	PORTD &= ~((1<<DIG1)|(1<<DIG2)|(1<<DIG3)|(1<<DIG4));
}

void showDigit(uint8_t digit_pos, uint8_t val){
	PORTD &= ~(1<<LATCH_PIN);
	shiftOut(val);
	PORTD |= (1<<LATCH_PIN);

	switch(digit_pos){
		case 0: PORTD |= (1<<DIG1); break;
		case 1: PORTD |= (1<<DIG2); break;
		case 2: PORTD |= (1<<DIG3); break;
		case 3: PORTD |= (1<<DIG4); break;
	}
}

void loadLOAD(void){
	display[0] = symb[0];
	display[1] = symb[1];
	display[2] = symb[2];
	display[3] = symb[3];
}

void loadPLAY(void){
	display[0] = symb[4];
	display[1] = symb[0];
	display[2] = symb[2];
	display[3] = symb[5];
}

void loadSTOP(void){
	display[0] = symb[6];
	display[1] = symb[7];
	display[2] = symb[1];
	display[3] = symb[4];
}

void shiftOut(uint8_t data){
	for(uint8_t i=0;i<8;i++){
		if(data & (1<<(7-i))) PORTD |= (1<<DATA_PIN);
		else PORTD &= ~(1<<DATA_PIN);
		PORTD |= (1<<CLOCK_PIN);
		PORTD &= ~(1<<CLOCK_PIN);
	}
}