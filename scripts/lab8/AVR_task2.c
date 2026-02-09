#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

#define DHT_PIN PB0

#define SEG_PORT PORTD
#define SEG_DDR  DDRD

#define DIG_PORT PORTC
#define DIG_DDR  DDRC
#define DIG_MASK 0x78  // PC3-PC6

#define RED_PIN PB3
#define GREEN_PIN PB2
#define BLUE_PIN PB1

uint8_t seg[10] = {
	0b00111111, //0
	0b00000110, //1
	0b01011011, //2
	0b01001111, //3
	0b01100110, //4
	0b01101101, //5
	0b01111101, //6
	0b00000111, //7
	0b01111111, //8
	0b01101111  //9
};

void setRGB(uint8_t r, uint8_t g, uint8_t b){
	if(r) PORTB |= (1<<RED_PIN); else PORTB &= ~(1<<RED_PIN);
	if(g) PORTB |= (1<<GREEN_PIN); else PORTB &= ~(1<<GREEN_PIN);
	if(b) PORTB |= (1<<BLUE_PIN); else PORTB &= ~(1<<BLUE_PIN);
}

void showDigit(uint8_t digit, uint8_t dot){
	uint8_t pattern = seg[digit];
	if(dot) pattern |= 0x80;
	SEG_PORT = pattern;
}

void displayDigit(uint8_t digit, uint8_t pos, uint8_t dot){
	DIG_PORT &= ~DIG_MASK;
	showDigit(digit, dot);
	DIG_PORT |= (1<<(pos+2));
	_delay_ms(2);
}


void displayAll(uint8_t t1,uint8_t t2,uint8_t h1,uint8_t h2){
	displayDigit(t1,1,0);
	displayDigit(t2,2,1);
	displayDigit(h1,3,0);
	displayDigit(h2,4,0);
}

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

void init_ports(){
	SEG_DDR = 0xFF;
	DIG_DDR |= DIG_MASK;
	DDRB |= (1<<RED_PIN)|(1<<GREEN_PIN)|(1<<BLUE_PIN);
}

int main(void){
	init_ports();
	float temperature=0, humidity=0;
	uint8_t t1=0,t2=0,h1=0,h2=0;
	uint16_t counter=0;

	while(1){
		displayAll(t1,t2,h1,h2);

		counter++;
		if(counter>=100){
			counter=0;
			if(DHT11_Read(&temperature,&humidity)){
				t1 = ((int)temperature)/10;
				t2 = ((int)temperature)%10;
				h1 = ((int)humidity)/10;
				h2 = ((int)humidity)%10;
			}
		}

		if(humidity<40) setRGB(0,0,1);
		else if(humidity<=60) setRGB(0,1,0);
		else setRGB(1,0,0);
	}
}
