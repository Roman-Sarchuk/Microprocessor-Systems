#define F_CPU 1000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <avr/interrupt.h>


#define CLOCK_PIN PD0
#define DATA_PIN PD1
#define LATCH_PIN PD2

#define DIG1 PD3
#define DIG2 PD4
#define DIG3 PD5
#define DIG4 PD6

volatile uint8_t current_digit = 1;
volatile uint16_t adc_value = 0;

const uint8_t segmentCodes[10] = {
	0xFC, 0x60, 0xDA, 0xF2, 0x66, 0xB6, 0xBE, 0xE0, 0xFE, 0xF6
};

void shiftOut(uint8_t data);
void disp(uint8_t number);
void disp_off();
uint16_t ADC_read(uint8_t channel);

int main(void){
	DDRD |= (1<<CLOCK_PIN)|(1<<DATA_PIN)|(1<<LATCH_PIN)|(1<<DIG1)|(1<<DIG2)|(1<<DIG3)|(1<<DIG4);
	
	ADMUX = (1<<REFS0);
	ADCSRA = (1<<ADEN)|(1<<ADPS1)|(1<<ADPS0);
	
	TCCR1A = 0;
	TCCR1B = (1<<CS10);
	TIMSK = (1<<TOIE1);
	TCNT1 = 0;
	
	sei();
	
	while (1){
		adc_value = ADC_read(1);
		_delay_ms(50);
	}
}

uint16_t ADC_read(uint8_t channel){
	ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);
	ADCSRA |= (1<<ADSC);
	while(ADCSRA & (1<<ADSC));
	return ADC;
}

ISR(TIMER1_OVF_vect){
	disp_off();
	switch(current_digit) {
		case 1: disp(adc_value / 1000); break;
		case 2: disp((adc_value / 100) % 10); break;
		case 3: disp((adc_value / 10) % 10); break;
		case 4: disp(adc_value % 10); break;
	}

	current_digit = (current_digit % 4) + 1;
}

void disp(uint8_t number) {
	shiftOut(segmentCodes[number]);

	switch(current_digit) {
		case 1: PORTD |= (1<<DIG1); break;
		case 2: PORTD |= (1<<DIG2); break;
		case 3: PORTD |= (1<<DIG3); break;
		case 4: PORTD |= (1<<DIG4); break;
	}
}

void disp_off() {
	PORTD &= ~((1<<DIG1)|(1<<DIG2)|(1<<DIG3)|(1<<DIG4));
}

void shiftOut(uint8_t data){
	PORTD &=~ (1<<LATCH_PIN);
	for(uint8_t i =0; i<8; i++){
		if(data &(1<<(7-i))) PORTD |= (1<<DATA_PIN);
		else PORTD &=~(1<<DATA_PIN);
		PORTD |= (1<<CLOCK_PIN);
		PORTD &= ~(1<<CLOCK_PIN);
	}
	PORTD |= (1<<LATCH_PIN);
}