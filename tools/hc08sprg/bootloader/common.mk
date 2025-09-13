
include .CPU

DEFINES += -DRAM_START=$(NATIVE_RAM_START)
DEFINES += -DRAM_SIZE=$(NATIVE_RAM_SIZE)
DEFINES += -DFLASH_START=$(FLASH_START)
DEFINES += -DFLASH_SIZE=$(NATIVE_FLASH_SIZE)

DEFINES += -DUSE_PLL_CLOCK=0 -DPLATFORM=9
DEFINES += -DFLASH_END=$(FLASH_END)

DEPEND = fancy_macros.asm board.asm platform.asm macros.asm bits68hc908.asm mc68$(CPU_VARIANT).asm reg68$(CPU_VARIANT).asm .CPU

MAKEFILES = Makefile $(wildcard ./*.mk)
DASM_OPTS = -f3 -E2 -S -T1 -v2


all: $(TARGET)-final.s19

$(TARGET).bin: $(TARGET).asm $(DEPEND) $(MAKEFILES)
	@echo dasm $* $(DEFINES) \-\> $@
	dasm $(TARGET).asm $(DASM_OPTS) -o$(TARGET).bin -l$(TARGET).lst -s$(TARGET).sym $(DEFINES)

clean:
	rm -f *.lst *.bin *.s19 *.sym flash.mk $(DEPEND) trace

include mklinks.mk

$(TARGET)-final.s19: $(TARGET).bin
	Bin2S19 -v -f $(TARGET).bin -c $(COLS) -b $(FLASH_END) $(VECTOR_MODE) -s >$(TARGET)-final.s19

include flash.mk

mklinks.mk:
	ln -s ../mklinks.mk

