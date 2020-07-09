#
# this Makefile serves as an example
#

# board name, important for bootloader configuration, used in subsequent Makefile
BOARD = crouzet_xt20_m2

# CPU Variant
VARIANT = mr32

# .CPU file will be created as a symbolic link 
include .CPU

# main source file name without extension, only one single file should be given, other needed *.c files have to be included in the main file
TARGET = testApp

# used to resolve dependecies, if Makefiles have changed we might need recompilation
MAKEFILES := Makefile $(wildcard ./*.mk)

# used header files from the generic include directory, mklinks.mk should state rule how to create the symbolic links
GENERICS = board.h peripherie.h config.h \
	project.h \
	uart_hc908.h timer_hc908.h adc_hc908.h i2c_hc908.h spi_hc908.h \
	printd_type.h uart_printh.h uart_printd.h uart_printb.h \
	types_hc908.h pintoggle.h token_pasting.h \
	bit_reverse.h keyMatrix.h HD44780_interface.h \
	m41t56.h truly_mcc315.h truly_mcc315.c \
	sliding_int_avg.h sliding_int_avg.c

# local files unique to the current project
PROJECT_HDRS = myFeatures.h myTypes.h crouzet_lcd_keybits.h display.h compiler_lib.h

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
# include moni08.mk

# flash with serial programmer+bootloader
include flash.mk

include s19.mk
include sdcc.mk

clean:
	rm -f $(TARGET).dep $(GENERICS) $(SDCC_DEPS) $(TARGET)-final.s19 $(TARGET).asm $(TARGET).cdb $(TARGET).rst $(TARGET).rel $(TARGET).sym $(TARGET).s19 *.bin $(TARGET).lk $(TARGET).lst $(TARGET).map

distclean: clean
	rm -f moni08.mk s19.mk sdcc.mk flash.mk .CPU .old_size objsize.pl kuser.sh

