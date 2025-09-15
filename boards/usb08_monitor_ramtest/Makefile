
BOARD = usb0_monitor_ramtest
VARIANT = jb8
REL_INC = ../..

BAUD = 9600
TTY = /dev/ttyS0

COLS = 1

include .CPU

VECTOR_MODE = -z $(FLASH_SIZE)

# START = $(NATIVE_RAM_START)
START = $(FLASH_START)

DEFINES += -DRAM_START=$(NATIVE_RAM_START)

ifeq (_$(START)_,_$(FLASH_START)_)
    DEFINES += -DFLASH_START=$(FLASH_START)
    DEFINES += -DFLASH_SIZE=$(FLASH_SIZE)
else
    DEFINES += -DFLASH_START=$(FLASH_START)
    DEFINES += -DFLASH_SIZE=$(NATIVE_FLASH_SIZE)
endif

DEFINES += -DSTART=$(START)

# DEFINES += -DPLATFORM=9

MKFILES = moni08.mk

GENERICS = macros.asm fancy_macros.asm bits68hc908.asm mc68hc908$(VARIANT).asm reg68hc908$(VARIANT).asm .CPU

DEPEND = board.asm platform.asm $(GENERICS)

TARGET = fastblink
# TARGET = memset

DASM_OPTS = -v4 -f3 -E2 -S -T1 -R

all: $(TARGET)-final.s19 

$(TARGET).bin: $(TARGET).asm $(DEPEND) Makefile $(MKFILES) mklinks.mk
	@echo dasm $(TARGET).asm \-\> $@
	dasm $(TARGET).asm $(DASM_OPTS) -o$(TARGET).bin -s$(TARGET).sym -l$(TARGET).lst $(DEFINES) || rm -f $(TARGET).bin

clean:
	rm -f *.bin *.s19 *.sym

distclean: clean
	rm -f *.lst $(MKFILES) $(GENERICS) $(DEPEND) .moni08rc .CPU

include mklinks.mk

$(TARGET)-final.s19: $(TARGET).bin
	Bin2S19 -v -f $(TARGET).bin -c $(COLS) -b $(START) $(VECTOR_MODE) -s >$(TARGET)-final.s19


include moni08.mk
