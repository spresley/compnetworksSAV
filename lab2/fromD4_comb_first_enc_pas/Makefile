# Modified by zhiyb (Yubo Zhi)

PRG	= FIRST_mic
SRC	= main_WITH_ENCRYPTION.cpp rfm12.cpp keypad_WITH_ENCRYPTION.cpp tone.cpp
LIB	= tft-cpp
LIBDIR	= ../lib
VERSION	= 0.0.1

PRGER	= usbasp
MCU_TARGET	= atmega644p
OPTIMIZE	= -Os

DEFS	+= -DF_CPU=12000000
DEFS	+= -D__PLATFORM_AVR__ -D__AVR_ATmega644P__
LIBS	+=
INCS	+=

EFUSE	= 0xFF
HFUSE	= 0x9C
LFUSE	= 0xE7

LOG	= ~/.make_avr.log

include ../Makefile.in
