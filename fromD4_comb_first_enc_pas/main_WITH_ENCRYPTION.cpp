#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <string.h>
#include "tft.h"
#include "tone.h"
#include "keypad.h"
#include "rgbconv.h"
#include "rfm12.h"
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//#include "encryption.h"


#define freq0 96	//the different frequency the rfm12 can change
#define freq1 519
#define freq2 942
#define freq3 1365
#define freq4 1788
#define freq5 2211
#define freq6 2634
#define freq7 3057
#define freq8 3480
#define freq9 3903


#define NUM_MENU 6


class tft_t tft;

uint8_t *bufptr;		//the pointer the buffer will return
char str[100];		//the string want to send
uint16_t p = 0;		//the position of string
void init_sys(void);	//initial the system
void text(void);		//founction for text
void main_menu(void);	//display and go to the main menu
void read(void);		//founction for read data from sd card
void help(void);		//founction for description the device
void about(void);	//device information
void play(void);		//get audio message from sd card
uint8_t detect(void);	//dectect whether receive messages
void sync(void);		//make host and slave sync.
uint8_t decrypt(uint8_t var, int key_no);//decrypt the message
void password();		//the password get into the device
void text_password();	//founction for typing password
void talk(void);		//push to talk
void init(void)		
{
	tft.init();		//init the TFT screen
	tft.setOrient(tft.Landscape);	//set the orientation
	tft.setBackground(conv::c32to16(0xFFFFFF));	//background colar white
	tft.setForeground(0x0000);	//forground colar black
	tft.clean();			//clean the TFT screen
	stdout = tftout(&tft);	
	tft.setBGLight(true);
}

int main(void)
{
	init_sys();
	DDRB |= 0x08;		//buzzer
	DDRB |= _BV(0);		//the flag give to the other ilmatto (push to talk)
	PORTB |= _BV(3);	
	PORTB |= _BV(0);
	_delay_ms(500);
	PORTB &= ~_BV(3);
	while(keypad::scan() == (uint8_t)-1);
	password();
	_delay_ms(100);  //little delay for the rfm12 to initialize properly
	rfm12_init();    //init the RFM12
	_delay_ms(100);
	sei();           //interrupts on

menu:
	_delay_ms(100);
	main_menu();
	uint8_t scan;
rescan:	while(( scan = keypad::scan()) == (uint8_t)-1);
	switch(scan)
	{
		case 0:
			goto send;
		case 1:
			goto read;
		case 2:
			goto play;
		case 4:
			goto help;
		case 5:
			goto sync;
		case 6:
			goto about;
		case 11:
			goto menu;
		case 15:
			goto talk;
		default:
			goto rescan;
	}
send:
	text();
	goto menu;
read:
	read();
	while(keypad::scan() != 11);
	goto menu;
play:
	play();
	while(keypad::scan() != 11);
	goto menu;
help:
	help();
	while(keypad::scan() != 11);
	goto menu;
sync:
	sync();
	while(keypad::scan() != 11);
	goto menu;
about:
	about();
	while(keypad::scan() != 11);
	goto menu;
talk:
	talk();
	goto menu;

	return 1;
}

static const char transform[10][5] = {
	{' ', '0', '!', 'L', '+'},
	{',', '.', '1', '-', '*'},
	{'a', 'b', 'c', '2', 'A'},
	{'d', 'e', 'f', '3', 'B'},
	{'g', 'h', 'i', '4', 'C'},
	{'j', 'k', 'l', '5', 'D'},
	{'m', 'n', 'o', '6', 'E'},
	{'p', 'q', 'r', 's', '7'},
	{'t', 'u', 'v', '8', 'O'},
	{'w', 'x', 'y', 'z', '9'},
};

static const char menu[6][10] = {
	"send",
	"read",
	"play",
	"help",
	"sync",
	"about"
};

void init_sys(void)
{
	init();
	init_tone();
	keypad::init();		//init the keypad
	keypad::init_timer();	
	tft.clean();
	tft.setZoom(3);
	tft.setBackground(conv::c32to16(0xFF0000));
	tft.clean();
	tft.setForeground(conv::c32to16(0x00FF00));
	puts(" *Welcome to use*");
	puts("");
	puts(" ***Group  O***");
	puts(" **Smart Talk**");
	tft.setZoom(1);
	puts("supported by OOS(Oblivion OS) version 1.0.0");
	puts("press any key to continue....");
}

void main_menu (void)
{
	tft.setBackground(conv::c32to16(0xFFFFFF));	//main menu gui part
	tft.setForeground(0x0000);
	tft.clean();
	tft.setZoom(2);
	uint8_t i = 0;
	uint32_t rec_c[] = {0xFF0000, 0x00FF00, 0x0000FF, 0xFFFF00, 0xFF00FF, 0x00FFFF, 0x123456, 0x654321, 0x765432, 0xFEDCBA};
	uint16_t rec_p[2] = {0,0};
	uint16_t rec_width = tft.width() / (NUM_MENU / 2);
	uint16_t rec_height = tft.height() / 2;
	uint16_t text_p[2] = {(rec_width - 5*FONT_WIDTH * tft.zoom())/2, rec_height/2}; //calculate the rectangle position and size, and the position of the words
	do{ 	//draw the rectangle
		for(uint8_t j = 0;j < NUM_MENU / 2; j++)
		{
			tft.rectangle(rec_p[0], rec_p[1], rec_width, rec_height, conv::c32to16(rec_c[i]));
			tft.setXY(rec_p[0] + text_p[0], rec_p[1] + text_p[1]);
			tft.setBackground(conv::c32to16(rec_c[i]));
			printf("%u",i+1);
			tft.setXY(rec_p[0] + text_p[0], rec_p[1] + text_p[1] + FONT_HEIGHT * tft.zoom());
			puts(menu[i]);
			rec_p[0] += rec_width;
			i++;
		}
		rec_p[0] = 0;
		rec_p[1] += rec_height;
	}while(i < NUM_MENU);
	while(keypad::scan() == (uint8_t) - 1)	//if get message, just goto display the messages
	{
		i = detect();
		if(i == 1)
		{
			while(keypad::scan() != 11);
			break;
		}
	}
	return;
}


void text(void)
{
	tft.setBackground(conv::c32to16(0xFFFFFF));
	tft.clean();
	tft.setZoom(2);
	tft.setForeground(conv::c32to16(0x00FF00));
	puts("***Enter the message***");
	puts("***********************");
	tft.setForeground(conv::c32to16(0x000000));
	tft.setZoom(2);
	while(1){
		uint8_t tem = keypad::scan();	//follwoing founction will be explained in keypad.cpp
		if(tem != -1)
		{	
			if(tem == 3)
			{
				keypad::send(str);
				for(uint16_t k = 0; k < p; k++)
					str[k] = '\0';
				p = 0;
			}
			else if(tem == 7)
				keypad::del(str, &p);
			else if(tem == 11)
				return;
			else if(tem == 14)
				keypad::buzzer();
			else if(tem < 3)
				keypad::num(tem + 1, 1, &p, str);
			else if (tem >=4 && tem < 7)
				keypad::num(tem, 0, &p, str);
			else if (tem >=8 && tem < 11)
				keypad::num(tem - 1, -1,  &p, str);
			else if(tem == 13)
				keypad::num(0, -13, &p, str);
			else if(tem == 12)
				keypad::pre_rec(str, &p);
		}
	}
}

void read(void)
{
	tft.clean();
	tft.setZoom(3);
	puts("read message!");
}
void play(void)
{
	tft.clean();
	tft.setZoom(3);
	puts("Choose file to play");
}
void help(void)	//help founction, keys description
{
h_menu:	tft.clean();
	tft.setZoom(3);
	puts("***Welcome to Use***");
	tft.setZoom(2);
	puts("The document helps users to use the device:");
	puts("press the number on the menu to enter the help document for specific function:");
	puts("1: send function");
	puts("2: read function");
	puts("3: play function");
	puts("4: sync function");
	puts("5: about function");
	while(keypad::scan() == (uint8_t)-1);
rescan:	uint8_t help_s = keypad::scan();
	switch(help_s)
	{
		case 0:
			tft.clean();
			puts("key description in send");
			puts("1: , . 1 - +");
			puts("2: a b c 2 A");
			puts("3: d e f 3 B");
			puts("4: g h i 4 C");
			puts("5: j k l 5 D");
			puts("6: m n o 6 E");
			puts("7: p q r s 7");
			puts("8: t u v 8 O");
			puts("9: w x y z 9");
			puts("0:   0 ! L +");
			puts("press 1 go to the next page");
			while(keypad::scan() != 0);
			tft.clean();
			puts("A: pre recoded message");
			puts("B: Buzzer");
			puts("C: Push to talk");
			puts("D: go back to main menu");
			puts("E: back space");
			puts("F: send messages");
			puts("press D go back to the help menu");
			while(keypad::scan() != 11);
			goto h_menu;
		case 1:
			tft.clean();
			puts("key description in read");
			puts("press D go back to the help menu");
			while(keypad::scan() != 11);
			goto h_menu;
		case 2:
			tft.clean();
			tft.setZoom(3);
			puts("key description in play");
			puts("press D go back to the help menu");
			while(keypad::scan() != 11);
			goto h_menu;
		case 4:
			tft.clean();
			tft.setZoom(3);
			puts("key description in ***");
			puts("press D go back to the help menu");
			while(keypad::scan() != 11);
			goto h_menu;
		case 5:
			tft.clean();
			tft.setZoom(3);
			puts("key description in about");
			tft.setZoom(2);
			puts("the device is build by group O");
			puts("press D go back to the help menu");
			while(keypad::scan() != 11);
			goto h_menu;
		case 11:
			return;
		default:
			goto rescan;
	}

}

void sync(void)
{
	tft.clean();
	tft.setZoom(2);
	puts("the device sync function");
	puts("press 1 sync");
	puts("press 2 to choose freq");
re_sync:	while(keypad::scan() == (uint8_t)-1);
	uint8_t tem = keypad::scan();
	switch(tem)
	{
		case 0:
			{
			puts("wait the device until see the commend");	
			uint8_t tv[] = "connected, press D go back";
			tft.clean();
			while(keypad::scan() != 11)
			{				
				if (rfm12_rx_status() == STATUS_COMPLETE)
				{
					bufptr = rfm12_rx_buffer(); //get the address of the current rx buffer

				// dump buffer contents to uart			
				for (uint8_t i=0;i<rfm12_rx_len();i++)
				{
					putchar(bufptr[i]);
				}
				
			//	puts("\"\r\n");
				
				// tell the implementation that the buffer
				// can be reused for the next data.
					rfm12_rx_clear();
				
				}
	
					puts(".");
				//	printf("%d", sizeof(tv));
					rfm12_tx (sizeof(tv), 0, tv);
			//	}
				//rfm12 needs to be called from your main loop periodically.
				//it checks if the rf channel is free (no one else transmitting), and then
				//sends packets, that have been queued by rfm12_tx above.
				rfm12_tick();	
				_delay_ms(1000); //small delay so loop doesn't run as fast
			}
			return;
			}
		case 1:
			{
				puts("press a number to choose a frequency band");
				while(keypad::scan() == (uint8_t)-1);
				uint8_t freq = keypad::scan();
				while(keypad::scan() == freq);
				switch(freq)
				{
				case 13:
					rfm12_livectrl(RFM12_LIVECTRL_FREQUENCY, freq0);
					puts("carrier frequency 0 selected");
					goto end;
				case 0:
					rfm12_livectrl(RFM12_LIVECTRL_FREQUENCY, freq1);
					puts("carrier frequency 1 selected");
					goto end;
				case 1:
					rfm12_livectrl(RFM12_LIVECTRL_FREQUENCY, freq2);
					puts("carrier frequency 2 selected");
					goto end;
				case 2:
					rfm12_livectrl(RFM12_LIVECTRL_FREQUENCY, freq3);
					puts("carrier frequency 3 selected");
					goto end;
				case 4:
					rfm12_livectrl(RFM12_LIVECTRL_FREQUENCY, freq4);
					puts("carrier frequency 4 selected");
					goto end;
				case 5:
					rfm12_livectrl(RFM12_LIVECTRL_FREQUENCY, freq5);
					puts("carrier frequency 5 selected");
					goto end;
				case 6:
					rfm12_livectrl(RFM12_LIVECTRL_FREQUENCY, freq6);
					puts("carrier frequency 6 selected");
					goto end;
				case 8:
					rfm12_livectrl(RFM12_LIVECTRL_FREQUENCY, freq7);
					puts("carrier frequency 7 selected");
					goto end;
				case 9:
					rfm12_livectrl(RFM12_LIVECTRL_FREQUENCY, freq8);
					puts("carrier frequency 8 selected");
					goto end;
				case 10:
					rfm12_livectrl(RFM12_LIVECTRL_FREQUENCY, freq9);
					puts("carrier frequency 9 selected");
					goto end;
				}
end:				puts("press D go back");
				while(keypad::scan() != 11);
				goto re_sync;
			}
		case 11:
			return;

		default:
			goto re_sync;
	}
}

void about(void)
{
	tft.clean();
	tft.setZoom(3);
	puts("key description in about");
	tft.setZoom(2);
	puts("the device is build by group O");
	puts("press D go back to the help menu");
//	while(keypad::scan() != 11);
}

uint8_t detect(void)
{
	
	if (rfm12_rx_status() == STATUS_COMPLETE)
		{
			tft.clean();
			PORTB |= _BV(3);
			bufptr = rfm12_rx_buffer(); //get the address of the current rx buffer

			// dump buffer contents to uart	
			uint8_t decrypted_data;		
			for (uint16_t i=0;i<rfm12_rx_len();i++)
			{
				/**********************************************************DECRYPTION************************************************/
				decrypted_data = decrypt(bufptr[i], i);

				putchar(decrypted_data);
				/*******************************************************************************************************************/
			}
			
			//puts("\"\r\n");
			puts(" ");
			puts("press D go back");
			// tell the implementation that the buffer
			// can be reused for the next data.
			rfm12_rx_clear();
			_delay_ms(500);
			PORTB &= ~_BV(3);
			return 1;
		}
	return 0;
}


void password(void)
{
re_pas:	char password[20] = {"a"};
	text_password();
	int wow = strcmp(password, str);
	if(!wow)
		{
			for(uint16_t k = 0; k < p; k++)
				str[k] = '\0';
			p = 0;
			return;
		}
	else
		{
				
			for(uint16_t k = 0; k < p; k++)
				str[k] = '\0';
			p = 0;
			puts("the password is wrong, press D to go back and re-enter the password");
			while(keypad::scan() != 11);
			goto re_pas;
		}
}

void text_password(void)
{
	//uint8_t p;
	tft.setBackground(conv::c32to16(0xFFFFFF));
	tft.clean();
	tft.setZoom(2);
	tft.setForeground(conv::c32to16(0x00FF00));
	puts("***Enter the password, and press F to confirm***");
	puts("***********************");
	tft.setForeground(conv::c32to16(0x000000));
	tft.setZoom(2);
	while(1){
		uint8_t tem = keypad::scan();
		if(tem != -1)
		{	
			if(tem == 3)
				return;
			else if(tem == 7)
				keypad::del(str, &p);
			else if(tem < 3)
				keypad::num(tem + 1, 1, &p, str);
			else if (tem >=4 && tem < 7)
				keypad::num(tem, 0, &p, str);
			else if (tem >=8 && tem < 11)
				keypad::num(tem - 1, -1,  &p, str);
			else if(tem == 13)
				keypad::num(0, -13, &p, str);		
		}
	//	printf ("%d\n",tem);
	}
}

void talk(void)
{
	while(keypad::scan() == 15)
	{
		PORTB &= ~_BV(0);
	}
	PORTB |= _BV(0);
}

void test_tft(void)	//for testing the TFT
{
	tft.clean();
	tft.setZoom(1);
	puts("*** TFT library testing ***");
	puts("STDOUT output, orientation, FG/BG colour, BG light");
	tft.setZoom(3);
	puts("Font size test");
	tft.setZoom(1);
	tft.setXY(300, 38);
	puts("Change text position & word warp test");
	tft.frame(115, 56, 200, 10, 2, 0xF800);
	puts("Draw frame test");
	tft.rectangle(118, 68, 180, 6, 0x07E0);
	puts("Draw rectangle test");
	tft.point(120, 76, 0x001F);
	tft.point(122, 76, 0x001F);
	tft.point(124, 76, 0x001F);
	tft.point(126, 76, 0x001F);
	tft.point(128, 76, 0x001F);
	tft.point(130, 76, 0x001F);
	puts("Draw points test");
	tft.line(200, 100, 300, 200, 0xF800);
	tft.line(300, 210, 200, 110, 0x001F);
	tft.line(200, 200, 300, 100, 0xF800);
	tft.line(300, 110, 200, 210, 0x001F);

	tft.line(100, 100, 300, 200, 0xF800);
	tft.line(300, 210, 100, 110, 0x001F);
	tft.line(100, 200, 300, 100, 0xF800);
	tft.line(300, 110, 100, 210, 0x001F);

	tft.line(200, 0, 300, 200, 0xF800);
	tft.line(300, 210, 200, 10, 0x001F);
	tft.line(200, 200, 300, 0, 0xF800);
	tft.line(300, 10, 200, 210, 0x001F);

	tft.line(100, 150, 300, 150, 0xF800);
	tft.line(300, 160, 100, 160, 0x001F);
	tft.line(250, 0, 250, 200, 0xF800);
	tft.line(260, 200, 260, 0, 0x001F);
	puts("Draw lines test");
}
uint8_t decrypt(uint8_t var, int key_no) 
{
	uint8_t key[3] = {0x36, 0x5A, 0xEB};
	return var ^ key[key_no % 3];
}

