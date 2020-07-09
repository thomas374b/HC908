
BOARD = crouzet_xt20_m2
VARIANT = mr32
REL_INC = ../../..

COLS = 32

include .CPU

VECTOR_MODE = -z $(FLASH_SIZE)

START = $(NATIVE_RAM_START)
# START = $(FLASH_START)

DEFINES += -DSTART=$(START)
DEFINES += -DRAM_START=$(RAM_START)
DEFINES += -DRAM_SIZE=$(RAM_SIZE)
DEFINES += -DFLASH_START=$(FLASH_START)
DEFINES += -DFLASH_SIZE=$(FLASH_SIZE)

DEFINES += -DUSE_PLL_CLOCK=1 
DEFINES += -DPLATFORM=9

GENERICS = macros.asm fancy_macros.asm bits68hc908.asm mc68hc908$(VARIANT).asm reg68hc908$(VARIANT).asm .CPU

DEPEND = board.asm $(GENERICS)

TARGET = fastblink

all: $(TARGET)-final.s19

$(TARGET).bin: $(TARGET).asm $(DEPEND) Makefile mklinks.mk
	@echo dasm $(TARGET).asm \-\> $@
	@dasm $(TARGET).asm -v3 -f3 -o$(TARGET).bin -l$(TARGET).lst $(DEFINES)

clean:
	rm -f *.bin *.s19

distclean: clean
	rm -f *.lst flash.mk $(GENERICS) .moni08rc moni08.mk board.asm

include mklinks.mk

$(TARGET)-final.s19: $(TARGET).bin
	Bin2S19 -v -f $(TARGET).bin -c $(COLS) -b $(START) $(VECTOR_MODE) -s >$(TARGET)-final.s19

ifeq (_$(START)_,_$(NATIVE_RAM_START)_)

BAUD = 4800
TTY = /dev/ttyS0

include moni08.mk

else

BAUD = 19200
TTY = /dev/ttyUSB0.

include flash.mk

endif



	