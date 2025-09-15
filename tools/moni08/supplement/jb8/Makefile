
BOARD = usb08
VARIANT = jb8
CPU = mc68hc908$(VARIANT)

include .CPU

START = $(RAM_START)

DEFINES += -DSTART=$(START)
DEFINES += -DRAM_START=$(NATIVE_RAM_START)
DEFINES += -DRAM_SIZE=$(NATIVE_RAM_SIZE)
DEFINES += -DFLASH_START=$(FLASH_START)
DEFINES += -DFLASH_SIZE=$(NATIVE_FLASH_SIZE)

DASM_OPTS = -f3 -E2 -S -T1 -v2

DEPEND = board.asm platform.asm macros.asm fancy_macros.asm bits68hc908.asm $(CPU).asm reg68hc908$(VARIANT).asm .CPU

all: hc908$(VARIANT)-bootloader.bin hc908$(VARIANT)-bulkErase.bin

%.bin: %.asm $(DEPEND) Makefile
	@echo dasm $* $(DEFINES) \-\> $@
	@dasm $*.asm $(DASM_OPTS) -o$@ -l$*.lst $(DEFINES)

clean:
	rm -f *.lst *.bin $(DEPEND) MONI*content* .moni08rc *.s19*

include ../.mklinks.mk

board.asm:
	ln -s $(REL_INC)/boards/$(BOARD)/board.asm .

platform.asm:
	ln -s $(REL_INC)/boards/$(BOARD)/platform.asm .


ifeq (_$(BAUD)_,__)
        BAUD = 9600
endif

ifeq (_$(TTY)_,__)
        TTY = /dev/ttyS0
endif

include ../../.device
include ../../.cache-$(DEVICE)

UMA = $(UNAME)-$(MACHINE)
MONI_EXE = ../../built/$(UMA)/moni08
MONI_08 = $(MONI_EXE) -v -b $(BAUD) -d $(TTY)

.moni08rc: $(MONI_EXE)
	@$(MONI_08) -M $(MHZ) -c $(CPU_VARIANT) --saverc

MONI08_RESET_OPT = -k -1 -P 3000

BTRBS = $(shell ls -l hc908$(VARIANT)-blink.bin | awk '{print $$5}')

blinktest: hc908$(VARIANT)-blink.bin .moni08rc zero64.bin
	$(MONI_08) $(MONI08_RESET_OPT) -a 0x48 -l hc908$(VARIANT)-blink.bin -a 0x0 -n 0x140 -r test-readback.bin -s -m 0xFE --exec --tty

readram: .moni08rc
	$(MONI_08)  -a $(NATIVE_RAM_START) -n $(NATIVE_RAM_SIZE) -r ram-readback.bin 

readback: .moni08rc
	$(MONI_08) --hash 255 -a $(FLASH_START) -n $(NATIVE_FLASH_SIZE) -r flash-readback.bin

MONIcontent.bin: .moni08rc
	$(MONI_08) -a $(MONI_START) -n $(MONI_SIZE) -r MONIcontent.bin

MONI2content.bin: .moni08rc
	$(MONI_08) -a $(MONI_2_START) -n $(MONI_2_SIZE) -r MONI2content.bin

MONIcontent.s19: MONIcontent.bin
	Bin2S19 -b $(MONI_START) -f MONIcontent.bin -s >MONIcontent.s19

MONI2content.s19: MONI2content.bin
	Bin2S19 -b $(MONI_2_START) -f MONI2content.bin -s >MONI2content.s19

MONI_CONTRIB_S19 = ../../contrib/hc908$(VARIANT)-monitor.s19

$(MONI_CONTRIB_S19): MONI2content.s19 MONIcontent.s19
	mkdir -p $(shell dirname $(MONI_CONTRIB_S19))
	grep S113 MONIcontent.s19 MONI2content.bin | cut -f2 -d: >$(MONI_CONTRIB_S19)

zero64.bin:
	dd if=/dev/zero of=zero64.bin bs=64 count=1

MONI08_RESET_OPT = -P 5000
BULK_ERASE_LOADER = $(CPU_VARIANT)-bulkErase.bin
BULK_ERASE_ARGS = -a $(NATIVE_RAM_START) -l $(BULK_ERASE_LOADER) --exec 

erase: $(BULK_ERASE_LOADER) .moni08rc
	$(MONI_08) -k -1 $(MONI08_RESET_OPT) $(BULK_ERASE_ARGS) -m $(FLASH_START)


