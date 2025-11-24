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

#define BUTTON PC0

volatile uint8_t current_digit = 1;
volatile uint16_t display_value = 0;
uint8_t last_button_state = 1;
uint16_t random_seed = 0;

const uint8_t segmentCodes[10] = {
	0xFC, 0x60, 0xDA, 0xF2, 0x66, 0xB6, 0xBE, 0xE0, 0xFE, 0xF6
};

void shiftOut(uint8_t data);
void disp(uint8_t number);
void disp_off();
uint16_t ADC_read(uint8_t channel);
uint16_t random16(void);

int main(void){
	DDRD |= (1<<CLOCK_PIN)|(1<<DATA_PIN)|(1<<LATCH_PIN)|(1<<DIG1)|(1<<DIG2)|(1<<DIG3)|(1<<DIG4);
	DDRC &= ~(1<<BUTTON);
	PORTC |= (1<<BUTTON);

	TCCR1A = 0;
	TCCR1B = (1<<CS10);
	TIMSK = (1<<TOIE1);
	TCNT1 = 0;

	ADMUX = (1<<REFS0);
	ADCSRA = (1<<ADEN)|(1<<ADPS1)|(1<<ADPS0);

	random_seed = ADC_read(0);
	sei();
	display_value = random16() % 10000;

	while(1){
		uint8_t current_button_state = (PINC & (1<<BUTTON)) ? 1 : 0;

		if(last_button_state && !current_button_state){
			_delay_ms(20);
			if(!(PINC & (1<<BUTTON))){
				display_value = random16() % 10000;
			}
		}

		last_button_state = current_button_state;
		_delay_ms(10);
	}
}

uint16_t ADC_read(uint8_t channel){
	ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);
	ADCSRA |= (1<<ADSC);
	while(ADCSRA & (1<<ADSC));
	return ADC;
}

uint16_t random16(void){
	random_seed = (random_seed * 1103515245 + 12345) % 32768;
	return random_seed;
}

ISR(TIMER1_OVF_vect){
	disp_off();
	switch(current_digit){
		case 1: disp(display_value / 1000); break;
		case 2: disp((display_value / 100) % 10); break;
		case 3: disp((display_value / 10) % 10); break;
		case 4: disp(display_value % 10); break;
	}
	current_digit = (current_digit % 4) + 1;
}

void disp(uint8_t number){
	shiftOut(segmentCodes[number]);
	switch(current_digit){
		case 1: PORTD |= (1<<DIG1); break;
		case 2: PORTD |= (1<<DIG2); break;
		case 3: PORTD |= (1<<DIG3); break;
		case 4: PORTD |= (1<<DIG4); break;
	}
}

void disp_off(){
	PORTD &= ~((1<<DIG1)|(1<<DIG2)|(1<<DIG3)|(1<<DIG4));
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