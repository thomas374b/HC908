
BOARD = relay7
VARIANT = jk3
BAUD = 7680
COLS = 16


include .CPU

# end of application flash, start of bootloader
FLASH_END = 0xFA80

# VECTOR_MODE_ARG = 65536 - FLASH_END - TABLE_SIZE - COLS
# VECTOR_MODE = -z 384

DEFINES += -DRAM_START=$(NATIVE_RAM_START)
DEFINES += -DRAM_SIZE=$(NATIVE_RAM_SIZE)
DEFINES += -DFLASH_START=$(FLASH_START)
DEFINES += -DFLASH_SIZE=$(FLASH_SIZE)

DEFINES += -DUSE_PLL_CLOCK=0 -DPLATFORM=9
DEFINES += -DFLASH_END=$(FLASH_END)

DEPEND = board.asm platform.asm macros.asm fancy_macros.asm bits68hc908.asm mc68hc908$(VARIANT).asm reg68hc908$(VARIANT).asm .CPU

TARGET = slfprgJK3

all: $(TARGET)-final.s19

%.bin: %.asm $(DEPEND) Makefile .mklinks.mk
	@echo dasm $* \-\> $@
	@dasm $*.asm -v3 -f3 -o$@ -l$*.lst $(DEFINES)

clean:
	rm -f *.lst *.bin *.s19 flash.mk $(DEPEND)

include .mklinks.mk

$(TARGET)-final.s19: $(TARGET).bin
	Bin2S19 -v -f $(TARGET).bin -c $(COLS) -b $(FLASH_END) $(VECTOR_MODE) -s >$(TARGET)-final.s19

include flash.mk