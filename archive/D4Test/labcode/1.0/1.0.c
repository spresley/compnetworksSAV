#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include "avrlcd.h"
#include "font.c"
#include "ili934x.c"
#include "lcd.c"

#include "rfm12.h"

int main ( void )
{
	init_lcd();
	set_orientation(East);
	display_string("Loading");

	uint8_t *bufptr;
	uint8_t i;
	uint8_t tv[] = "foobar";

	uint16_t ticker = 0;
	
	display_string(".");
	_delay_ms(100);  //little delay for the rfm12 to initialize properly
	rfm12_init();    //init the RFM12
	display_string(".");
	
	sei();           //interrupts on

	display_string("RFM12 Pingpong test");

	while (42) //while the universe and everything
	{
		if (rfm12_rx_status() == STATUS_COMPLETE)
		{
			//so we have received a message

			//blink the LED
			#ifdef LED_PORT
				LED_PORT ^= _BV(LED_BIT);
			#endif

			display_string("new packet: \"");

			bufptr = rfm12_rx_buffer(); //get the address of the current rx buffer

			// dump buffer contents to uart			
			for (i=0;i<rfm12_rx_len();i++)
			{
				display_string( bufptr[i] );
			}
			
			display_string("\"\r\n");
			
			// tell the implementation that the buffer
			// can be reused for the next data.
			rfm12_rx_clear();
		}


		ticker ++;
		if(ticker == 3000){
			ticker = 0;
			display_string(".\r\n");
			rfm12_tx (sizeof(tv), 0, tv);
		}

		//rfm12 needs to be called from your main loop periodically.
		//it checks if the rf channel is free (no one else transmitting), and then
		//sends packets, that have been queued by rfm12_tx above.
		rfm12_tick();
		
		_delay_us(100); //small delay so loop doesn't run as fast
	}
}
