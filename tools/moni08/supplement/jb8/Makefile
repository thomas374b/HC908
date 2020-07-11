
BOARD = usb08
VARIANT = jb8
CPU = mc68hc908$(VARIANT)

include .CPU

START = $(NATIVE_RAM_START)

DEFINES += -DSTART=$(START)
DEFINES += -DRAM_START=$(NATIVE_RAM_START)
DEFINES += -DRAM_SIZE=$(NATIVE_RAM_SIZE)
DEFINES += -DFLASH_START=$(FLASH_START)
DEFINES += -DFLASH_SIZE=$(FLASH_SIZE)

DEFINES += -DUSE_PLL_CLOCK=0 -DPLATFORM=0

DASM_OPTS = -f3 -E2 -S -T1 -v2

DEPEND = board.asm macros.asm fancy_macros.asm bits68hc908.asm $(CPU).asm reg68hc908$(VARIANT).asm .CPU

all: hc908$(VARIANT)-bootloader.bin hc908$(VARIANT)-bulkErase.bin

%.bin: %.asm $(DEPEND) Makefile
	@echo dasm $* $(DEFINES) \-\> $@
	@dasm $*.asm $(DASM_OPTS) -o$@ -l$*.lst $(DEFINES)

clean:
	rm -f *.lst *.bin $(DEPEND)

include ../.mklinks.mk

board.asm:
	ln -s $(REL_INC)/boards/$(BOARD)/board.asm .



















