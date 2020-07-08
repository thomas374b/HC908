
include .CPU

START = $(RAM_START)

DEFINES += -DSTART=$(START)
DEFINES += -DRAM_START=$(RAM_START)
DEFINES += -DRAM_SIZE=$(RAM_SIZE)
DEFINES += -DFLASH_START=$(FLASH_START)
DEFINES += -DFLASH_SIZE=$(FLASH_SIZE)

DEPEND = board.asm macros.asm fancy_macros.asm bits68hc908.asm mc68hc908jb8.asm reg68hc908jb8.asm .CPU

all: hc908jb8-bootloader.bin hc908jb8-bulkErase.bin

%.bin: %.asm $(DEPEND) Makefile
	@echo dasm $* \-\> $@
	@dasm $*.asm -v3 -f3 -o$@ -l$*.lst $(DEFINES)

clean:
	rm -f *.lst *.bin $(DEPEND)

include .mklinks.mk



