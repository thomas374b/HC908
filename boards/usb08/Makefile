
BOARD = usb08
VARIANT = jb8
REL_INC = ../..

TTY = /dev/ttyS0
WIRE = S
BAUD = 9600
COLS = 2

include .CPU

# VECTOR_MODE = -z $(FLASH_SIZE)

DEFINES += -DSTART=$(FLASH_START)
DEFINES += -DRAM_START=$(RAM_START)
DEFINES += -DRAM_SIZE=$(RAM_SIZE)
DEFINES += -DFLASH_START=$(FLASH_START)
DEFINES += -DFLASH_SIZE=$(FLASH_SIZE)

# DEFINES += -DUSE_PLL_CLOCK=0 

GENERICS = macros.asm fancy_macros.asm bits68hc908.asm mc68hc908$(VARIANT).asm reg68hc908$(VARIANT).asm .CPU

DEPEND = board.asm platform.asm $(GENERICS)

MAKEFILES = Makefile $(wildcard ./*.mk)

SYMBOLS = -S -T1 -s$(TARGET).sym
DASM_OPTS = -f3 -E2 -R -v2  $(SYMBOLS) # -d7 # $(SYMBOLS) -v4 -d7
# DASM_OPTS = -f3 -E2 -S -T1 -R -v2

TARGET = sensors

all: $(TARGET)-final.s19

$(TARGET).bin: $(TARGET).asm $(DEPEND) $(MAKEFILES)
	@echo dasm $(TARGET).asm \-\> $@
	dasm $(TARGET).asm $(DASM_OPTS) -o$(TARGET).bin -l$(TARGET).lst $(DEFINES)

clean:
	rm -f *.bin *.s19 *.lst *.sym $(GENERICS)

distclean: clean
	rm -f *.lst flash.mk $(GENERICS)

include mklinks.mk

$(TARGET)-final.s19: $(TARGET).bin
	Bin2S19 -v -f $(TARGET).bin -c $(COLS) -b $(FLASH_START) $(VECTOR_MODE) -s >$(TARGET)-final.s19

include flash.mk

