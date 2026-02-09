#define F_CPU 20000000UL
#include <avr/io.h>
#include <util/delay.h>

#define CS_PIN  PD2
#define DIN_PIN PD1
#define CLK_PIN PD0

#define CS_LOW()  (PORTD &= ~(1<<CS_PIN))
#define CS_HIGH() (PORTD |=  (1<<CS_PIN))
#define CLK_LOW() (PORTD &= ~(1<<CLK_PIN))
#define CLK_HIGH() (PORTD |=  (1<<CLK_PIN))
#define DIN_LOW() (PORTD &= ~(1<<DIN_PIN))
#define DIN_HIGH()(PORTD |=  (1<<DIN_PIN))
#define SPI_DELAY() _delay_us(1)

// Send a byte via software SPI
void SPI_Write_Byte(uint8_t data)
{
	for(int i=0;i<8;i++)
	{
		if(data & 0x80) DIN_HIGH(); else DIN_LOW();
		CLK_HIGH();
		SPI_DELAY();
		CLK_LOW();
		SPI_DELAY();
		data <<= 1;
	}
}

// Send a command to MAX7219
void MAX7219_Write(uint8_t reg, uint8_t data)
{
	CS_LOW();
	SPI_Write_Byte(reg);
	SPI_Write_Byte(data);
	CS_HIGH();
}

// Initialize MAX7219
void MAX7219_Init()
{
	DDRD |= (1<<PD0)|(1<<PD1)|(1<<PD2); // SPI ports as output
	MAX7219_Write(0x09,0x00); // Decode mode: none
	MAX7219_Write(0x0A,0x08); // Intensity
	MAX7219_Write(0x0B,0x07); // Scan limit: all 8 rows
	MAX7219_Write(0x0C,0x01); // Normal operation
	MAX7219_Write(0x0F,0x00); // Display test off
}

// Clear the matrix
void MAX7219_Clear()
{
	for(uint8_t i=1;i<=8;i++) MAX7219_Write(i,0x00);
}

// Send a full frame
void display_frame(uint8_t frame[8])
{
	for(uint8_t row=0; row<8; row++)
	{
		MAX7219_Write(row+1, frame[row]);
	}
}

// Moving dot along the contour
void dot_on_contour()
{
	uint8_t frame[8];

	// Define dot coordinates along the contour (8x8)
	uint8_t contourX[28] = {0,1,2,3,4,5,6,7, 7,7,7,7,7,7,7, 6,5,4,3,2,1,0,0,0,0,0,0,0};
	uint8_t contourY[28] = {0,0,0,0,0,0,0,0, 1,2,3,4,5,6,7, 7,7,7,7,7,7,7,6,5,4,3,2,1,0};

	for(int i=0; i<28; i++)
	{
		for(int r=0; r<8; r++) frame[r] = 0x00; // Clear the frame
		frame[contourY[i]] = (1 << contourX[i]); // Light up the current dot
		display_frame(frame);
		_delay_ms(150); // Dot movement speed
	}
}

int main(void)
{
	MAX7219_Init();
	MAX7219_Clear();

	while(1)
	{
		dot_on_contour();
	}
}
