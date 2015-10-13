#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

#define FREQ 262

void init_tone(void)
{
	TCCR1A = _BV(COM1A0) | _BV(COM1A1);	// Set OC1A on Compare Match
	TCCR1C = 0;				// No force output
	TCCR1B = _BV(WGM13);			// PWM, PFC, TOP: ICR1
	TCCR1B |= _BV(CS11) /*| _BV(CS10)*/;	// clk/8 prescaler
	TIMSK1 = 0;				// No interrupts
	TCNT1H = 0x00;				// Counter 1
	TCNT1L = 0x00;
	ICR1H = 0x00;				// Top register
	ICR1L = 0x00;
	OCR1AH = 0x00;				// Compare register
	OCR1AL = 0x00;
	DDRA |= _BV(0);				// Output
}

/* tone ---
   Set oscillator output to desired frequency
*/
void tone(uint16_t frequency)
{
	frequency = F_CPU / 8 / 2 / frequency;
	ICR1H = frequency / 0x0100;		// Compare register
	ICR1L = frequency % 0x0100;
	frequency /= 2;
	OCR1AH = frequency / 0x0100;		// Duty cycle
	OCR1AL = frequency % 0x0100;
}
