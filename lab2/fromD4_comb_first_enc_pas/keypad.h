#include <avr/io.h>
#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include "tft.h"
namespace keypad
{
	void init(void);
	void init_timer(void);
	uint8_t scan(void);
	void num(uint8_t i, int a,  uint16_t *p, char *str);
	void send(char *str);
	void del(char *str, uint16_t *p);
	void pre_rec(char *str, uint16_t *p);
	void exit(void);
	void buzzer(void);
	void talk(char *str, uint16_t *p);
}
