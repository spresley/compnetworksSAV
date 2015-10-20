#define CALLSIGN D
#define STOPBIT #

#include <avr/io.h>
#include <stdio.h>
#include <string.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "tft.h"
#include "rfm12.h"
//#include "spi.h"

#define NUM_MENU 6

uint8_t *bufptr;
char str[100];	//the string want to send
uint16_t p = 0;	//the position of string
void init_sys(void);
uint8_t detect(void);
void sync(void);

void init(void)
{
//	DDRB |= 0x80;			// LED
//	PORTB |= 0x80;
	tft::init();
	tft::setOrient(tft::Landscape | tft::Flipped);
	tft::background = COLOUR_565_888(0xFFFFFF);
	tft::foreground = 0x0000;
	tft::clean();
	stdout = tftout();
	tft::setBGLight(true);
}

int main(void)
{
	init_sys();
	_delay_ms(100);  //little delay for the rfm12 to initialize properly
	rfm12_init();    //init the RFM12
	_delay_ms(100);
	uint8_t tv[10] = "Connecting";
	uint8_t count = 0;
	sei();           //interrupts on
	while(1)
	{
		if (rfm12_rx_status() == STATUS_COMPLETE)
		{
			bufptr = rfm12_rx_buffer(); //get the address of the current rx buffer

			// dump buffer contents to uart			
			for (uint8_t i=0;i<rfm12_rx_len();i++)
			{
				putchar(bufptr[i]);
			}
				
				// tell the implementation that the buffer
				// can be reused for the next data.
				rfm12_rx_clear();
				//_delay_ms(1000);
		}
				
				
	
					puts(".");
				//	printf("%d", sizeof(tv));
				//	printf("02%x",rfm12_tx(sizeof(tv), 0, tv)));
				switch count
				{
					case 0:
						strcpy (tv,"NHi Nath!#");
						count++;
						break;
					case 1:
						strcpy (tv,"SHi Sam!!#");
						count++;
						break;
					case 2:
						strcpy (tv,"HHuw haha#");
						count = 0;
						break;
				}

				rfm12_tx(sizeof(tv), 0, tv);	
			//	}
				//rfm12 needs to be called from your main loop periodically.
				//it checks if the rf channel is free (no one else transmitting), and then
				//sends packets, that have been queued by rfm12_tx above.
			for (uint8_t j = 0; j < 100; j++)	
			{	
				rfm12_tick();	
				_delay_us(500); 
			}
			_delay_ms(500);	//small delay so loop doesn't run as fast
			
	}
}

void init_sys(void)
{
	init();
	tft::clean();
	tft::zoom = 3;
	tft::background = COLOUR_565_888(0xFF0000);
	tft::clean();
	tft::foreground = COLOUR_565_888(0x00FF00);
	puts(" *Welcome to use*");
	puts("");
	puts(" ***Group  O***");
	puts(" **Smart Talk**");
	tft::zoom = 1;
	puts("supported by OOS(Oblivion OS) version 1.0.0");
	puts("press any key to continue....");
	
}

