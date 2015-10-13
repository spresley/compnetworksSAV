/*
 * HelloWorld.c
 *
 *  Created on: 11 Dec 2013
 *      Author: dm6g13
 */

#include "avrlcd.h"
#include "font.c"
#include "ili934x.c"
#include "lcd.c"
#include <avr/io.h>
#include <util/delay.h>

int main()
{
	init_lcd();
	set_orientation(East);
	display_string("Hello Nathan");
	return 0;
}
