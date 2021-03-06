PRG            = image
OBJ            = 1.0.o rfm12.o

include ../make_config

# these are set by config in parent dir
#MCU_TARGET     = atmega644p
#DEFS           = -DF_CPU=12000000



OPTIMIZE       = -Os

FLASHCMD       = avrdude -c usbasp -p $(MCU_TARGET) -U flash:w:$(PRG).hex
ERASECMD       = uisp -dprog=bsd --erase
LAUNCH_BOOTLOADER = foodloader-0.21/launcher/launch-bootloader

#SERIAL = /dev/ttyS0
SERIAL = /dev/ttyUSB0


LIBS           =

# You should not have to change anything below here.

CC             = avr-gcc
SRC = $(OBJ:%.o=%.c)
#DEPENDFILE = .depend

# Override is only needed by avr-lib build system.

override CFLAGS        =  -g -Wall $(OPTIMIZE) -fno-jump-tables -mmcu=$(MCU_TARGET) -D __PLATFORM_AVR_ $(DEFS)
override LDFLAGS       = -Wl,-Map,$(PRG).map

OBJCOPY        = avr-objcopy
OBJDUMP        = avr-objdump

all: dep $(PRG).elf lst text eeprom

$(PRG).elf: $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -rf *.o $(PRG).elf $(UART_LIB_DIR)/*.o
	rm -rf *.lst *.map $(EXTRA_CLEAN_FILES)

dep: $(SRC)
	$(CC) -mmcu=$(MCU_TARGET) -D __PLATFORM_AVR_ $(DEFS) -MM $(SRC)

# > $(DEPENDFILE)

#-include $(DEPENDFILE)

size: image.elf
	avr-nm -S --size-sort image.elf
	avr-size *.o *.elf

flash: all
	$(FLASHCMD)

sflash: all
	$(LAUNCH_BOOTLOADER) $(SERIAL) 115200
	avrdude -p m8 -b 115200 -u -c avr109 -P $(SERIAL) -U f:w:$(PRG).hex -F

sflasht:
	$(LAUNCH_BOOTLOADER) $(SERIAL) 115200
	avrdude -p m8 -b 115200 -u -c avr109 -P $(SERIAL) -t

lst:  $(PRG).lst

%.lst: %.elf
	$(OBJDUMP) -h -S $< > $@

# Rules for building the .text rom images

text: hex bin srec

hex:  $(PRG).hex
bin:  $(PRG).bin
srec: $(PRG).srec

%.hex: %.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

%.srec: %.elf
	$(OBJCOPY) -j .text -j .data -O srec $< $@

%.bin: %.elf
	$(OBJCOPY) -j .text -j .data -O binary $< $@

# Rules for building the .eeprom rom images

eeprom: ehex ebin esrec

ehex:  $(PRG)_eeprom.hex
ebin:  $(PRG)_eeprom.bin
esrec: $(PRG)_eeprom.srec

%_eeprom.hex: %.elf
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O ihex $< $@

%_eeprom.srec: %.elf
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O srec $< $@

%_eeprom.bin: %.elf
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O binary $< $@

# Every thing below here is used by avr-libc's build system and can be ignored
# by the casual user.

FIG2DEV                 = fig2dev
EXTRA_CLEAN_FILES       = *.hex *.bin *.srec

dox: eps png pdf

eps: $(PRG).eps
png: $(PRG).png
pdf: $(PRG).pdf

%.eps: %.fig
	$(FIG2DEV) -L eps $< $@

%.pdf: %.fig
	$(FIG2DEV) -L pdf $< $@

%.png: %.fig
	$(FIG2DEV) -L png $< $@
