// Lab1.p2 - running Shadow
// pins&HL: 9, 5, 7, 10, 2, 3
// 2-PD2, 3-PD3, 5-PD5, 7-PD7, 9-PB2, 10-PB3

#define DELAY 200
#define NUM_LEDS 6

void setup() {
  // setup pins
  DDRB |= (1 << PB1) | (1 << PB2);               // D9, D10
  DDRD |= (1 << PD2) | (1 << PD3) | (1 << PD5) | (1 << PD7); // D1, D2, D5, D7
}

void loop() {
  for (uint8_t i = 0; i < NUM_LEDS; i++) {
    // turn on all
    PORTB |= (1 << PB1) | (1 << PB2);
    PORTD |= (1 << PD2) | (1 << PD3) | (1 << PD5) | (1 << PD7);

    // turn off one
    switch (i) {
      case 0: PORTB &= ~(1 << PB1); break; // D9
      case 1: PORTD &= ~(1 << PD5); break; // D5
      case 2: PORTD &= ~(1 << PD7); break; // D7
      case 3: PORTB &= ~(1 << PB2); break; // D10
      case 4: PORTD &= ~(1 << PD2); break; // D1
      case 5: PORTD &= ~(1 << PD3); break; // D2
    }

    delay(DELAY);
  }
}
