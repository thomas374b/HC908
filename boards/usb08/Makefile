
BOARD = usb08
VARIANT = jb8
REL_INC = ../..

BAUD = 9600
COLS = 32

include .CPU

VECTOR_MODE = -z $(FLASH_SIZE)

DEFINES += -DSTART=$(FLASH_START)
DEFINES += -DRAM_START=$(RAM_START)
DEFINES += -DRAM_SIZE=$(RAM_SIZE)
DEFINES += -DFLASH_START=$(FLASH_START)
DEFINES += -DFLASH_SIZE=$(FLASH_SIZE)

# DEFINES += -DUSE_PLL_CLOCK=0 

GENERICS = macros.asm fancy_macros.asm bits68hc908.asm mc68hc908$(VARIANT).asm reg68hc908$(VARIANT).asm .CPU

DEPEND = board.asm platform.asm $(GENERICS)

TARGET = sensors

all: $(TARGET)-final.s19

$(TARGET).bin: $(TARGET).asm $(DEPEND) Makefile .mklinks.mk
	@echo dasm $(TARGET).asm \-\> $@
	@dasm $(TARGET).asm -v3 -f3 -o$(TARGET).bin -l$(TARGET).lst $(DEFINES)

clean:
	rm -f *.bin *.s19

distclean: clean
	rm -f *.lst flash.mk $(GENERICS)

include mklinks.mk

$(TARGET)-final.s19: $(TARGET).bin
	Bin2S19 -v -f $(TARGET).bin -c $(COLS) -b $(FLASH_START) $(VECTOR_MODE) -s >$(TARGET)-final.s19

include flash.mk