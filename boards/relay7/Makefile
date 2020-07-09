#
# this Makefile serves as an example
#

# board name, important for bootloader configuration, used in subsequent Makefile
BOARD = relay7

# CPU Variant
VARIANT = jk3

# .CPU file will be created as a symbolic link 
include .CPU

# main source file name without extension, only one single file should be given, other needed *.c files have to be included in the main file
TARGET = relay7

# used to resolve dependecies, if Makefiles have changed we might need recompilation
MAKEFILES := Makefile $(wildcard ./*.mk)

# used header files from the generic include directory, mklinks.mk should state rule how to create the symbolic links
GENERICS = timer_hc908.h adc_hc908.h dumb_uart.c project.h types_hc908.h pintoggle.h nakedISR_hc908.h token_pasting.h

# local files unique to the current project
PROJECT_HDRS = peripherie.h config.h myFeatures.h myTypes.h byte8_average.c dumb_uart.h

# needed source files from the SDCC compiler libraries
SDCC_DEPS = _mulint.c _divsint.c _modsint.c _moduint.c _divuint.c

# list all sources to resolve dependencies
DEPEND = $(GENERICS) $(PROJECT_HDRS) $(SDCC_DEPS)

HEADERS  := $(wildcard ./*.h)

all: $(TARGET).s19
	@echo $?

install: $(TARGET)-final.s19
	make flash

include mklinks.mk

# flash in monitor mode
include moni08.mk

# flash with serial programmer+bootloader
# include flash.mk

include s19.mk
include sdcc.mk

clean:
	rm -f $(TARGET).dep $(GENERICS) $(SDCC_DEPS) $(TARGET)-final.s19 $(TARGET).asm $(TARGET).cdb $(TARGET).rst $(TARGET).rel $(TARGET).sym $(TARGET).s19 *.bin $(TARGET).lk $(TARGET).lst $(TARGET).map

distclean: clean
	rm -f moni08.mk s19.mk sdcc.mk flash.mk .CPU .old_size objsize.pl

