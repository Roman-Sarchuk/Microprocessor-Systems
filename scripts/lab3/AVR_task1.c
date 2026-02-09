#define F_CPU 1000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define BLED PB1
#define GLED PB2
#define RLED PB3
#define BUTTON PD2


volatile uint8_t ledMode = 0;
volatile uint16_t last_interrupt_time = 0;
volatile uint16_t current_time = 0;


void setMode(uint8_t mode) {
    uint8_t mask = (1 << RLED) | (1 << GLED) | (1 << BLED);
    PORTB &= ~mask;

    switch (mode) {
	    case 1: PORTB |= (1 << RLED); break;		// Red
	    case 2: PORTB |= (1 << GLED); break;		// Green
	    case 3: PORTB |= (1 << BLED); break;		// Blue
	    case 4: PORTB |= (1 << RLED) | (1 << BLED); break;		// Purple (Red + Blue)
	    case 5: PORTB |= (1 << GLED) | (1 << BLED); break;		// Turquoise (Blue + Green)
	    case 6: PORTB |= (1 << RLED) | (1 << GLED); break;		// Orange (Red + Green)
	    case 7: PORTB |= (1 << RLED) | (1 << GLED) | (1 << BLED); break;		// White (Red + Green + Blue)
	    default: break;		// 0 or others
    }
}


ISR(INT0_vect) {
	uint16_t interrupt_time = current_time;
	if (interrupt_time - last_interrupt_time > 10) {
		ledMode++;
		if (ledMode == 8) ledMode = 0;
	}
	last_interrupt_time = interrupt_time;
}

ISR(TIMER0_OVF_vect) {
	current_time++;
}


int main(void) {
	// set up ports
	DDRB |= (1 << RLED) | (1 << GLED) | (1 << BLED);
	PORTB = 0x00;
	DDRD &= ~(1 << BUTTON);
	PORTD |= (1 << BUTTON);
	
	// set up interruptions
	MCUCR |= (1 << ISC01) | (1 << ISC00);
	GICR |= (1 << INT0);
	
	// set up timer
	TCCR0 = (1 << CS01) | (1 << CS00);
	TIMSK |= (1 << TOIE0);
	
	sei();
	
	// loop
	while (1) {
		setMode(ledMode);
		_delay_ms(20);
	}
}