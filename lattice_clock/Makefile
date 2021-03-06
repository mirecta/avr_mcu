# Name: Makefile
# Project: hid-mouse example
# Author: Christian Starkjohann
# Creation Date: 2008-04-07
# Tabsize: 4
# Copyright: (c) 2008 by OBJECTIVE DEVELOPMENT Software GmbH
# License: GNU GPL v2 (see License.txt), GNU GPL v3 or proprietary (CommercialLicense.txt)
# This Revision: $Id: Makefile 692 2008-11-07 15:07:40Z cs $

DEVICE  = atmega8
F_CPU   = 8000000	# in Hz
FUSE_L  = # see below for fuse values for particular devices
FUSE_H  = 
AVRDUDE = sudo avrdude -c usbasp   -p $(DEVICE) # edit this line for your programmer

CFLAGS  = 
OBJECTS = main.o dht22.o
DEPS = $(OBJECTS:.o=.d)

COMPILE = avr-gcc -Wall -g -Os -DF_CPU=$(F_CPU) $(CFLAGS) -mmcu=$(DEVICE)

all:	hex

# symbolic targets:
help:
	@echo "This Makefile has no default rule. Use one of the following:"
	@echo "make hex ....... to build main.hex"
	@echo "make program ... to flash fuses and firmware"
	@echo "make fuse ...... to flash the fuses"
	@echo "make flash ..... to flash the firmware (use this on metaboard)"
	@echo "make clean ..... to delete objects and hex file"

hex: main.hex

program: flash fuse

# rule for uploading firmware:
flash: main.hex
	$(AVRDUDE) -U flash:w:main.hex:i

fuses:
	$(AVRDUDE) -U lfuse:w:0x94:m -U hfuse:w:0xd9:m

# rule for deleting dependent files (those which can be built by Make):
clean:
	rm -f main.hex main.lst main.obj main.cof main.list main.map main.eep.hex main.elf *.o *.d 

# Generic rule for compiling C files:
.c.o:
	$(COMPILE) -c $< -o $@
	$(COMPILE) -MM $*.c > $*.d 

.cc.o:
	$(COMPILE) -c $< -o $@
	$(COMPILE) -MM $*.cc > $*.d 
    
# Generic rule for assembling Assembler source files:
.S.o:
	$(COMPILE) -x assembler-with-cpp -c $< -o $@
# "-x assembler-with-cpp" should not be necessary since this is the default
# file type for the .S (with capital S) extension. However, upper case
# characters are not always preserved on Windows. To ensure WinAVR
# compatibility define the file type manually.

# Generic rule for compiling C to assembler, used for debugging only.
.c.s:
	$(COMPILE) -S $< -o $@

# file targets:


main.elf:  $(OBJECTS)	# usbdrv dependency only needed because we copy it
	$(COMPILE) -o main.elf $(OBJECTS) -lc

main.hex: main.elf
	rm -f main.hex main.eep.hex
	avr-objcopy -j .text -j .data -O ihex main.elf main.hex
	avr-size main.hex

# debugging targets:

disasm:	main.elf
	avr-objdump -S main.elf

cpp:
	$(COMPILE) -E main.c


-include	$(DEPS)
