
include .CPU

START = $(RAM_START)

DEFINES += -DSTART=$(START)
DEFINES += -DRAM_START=$(NATIVE_RAM_START)
DEFINES += -DRAM_SIZE=$(NATIVE_RAM_SIZE)
DEFINES += -DFLASH_START=$(FLASH_START)
DEFINES += -DFLASH_SIZE=$(FLASH_SIZE)

DEFINES += -DUSE_PLL_CLOCK=0 -DPLATFORM=0

DEPEND = board.asm macros.asm fancy_macros.asm bits68hc908.asm mc68hc908mr32.asm reg68hc908mr32.asm .CPU

all: hc908mr32-bootloader.bin hc908mr32-bulkErase.bin

%.bin: %.asm $(DEPEND) monitor-excerpts.asm Makefile
	@echo dasm $* \-\> $@
	@dasm $*.asm -v3 -f3 -o$@ -l$*.lst $(DEFINES)

clean:
	rm -f *.lst *.bin $(DEPEND)

include .mklinks.mk



