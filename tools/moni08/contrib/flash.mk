
ifeq (_$(BAUD)_,__)
	BAUD = 7680
endif

ifeq (_$(TTY)_,__)
	TTY = /dev/ttyS0
endif

ifeq (_$(REL_INC)_,__)
	REL_INC = ../../..
endif

POWER_ON_GAP = 5000
ERASE_READY_GAP = 2000


ifeq (_$(MONI08_RESET_OPT)_,__)
ifeq (_$(HAVE_DTR_RESET)_,__)
    ifeq (_$(HAVE_RTS_RESET)_,__)
	# no machine controllable reset, wait 5 seconds to give the user to push a button or switch power on
	MONI08_RESET_OPT = -P $(POWER_ON_GAP)
    else
	# reset via RTS, hold 5 ms low
	MONI08_RESET_OPT = -t 8 -g 5 -t 10
    endif
else
    # reset via DTR, hold 5ms low
    MONI08_RESET_OPT = -t 4 -g 5 -t 5
endif
endif

UNAME = $(shell uname)
MACHINE = $(shell uname -m | sed s/x86_64/x86-64/g)
UMA = $(UNAME)-$(MACHINE)

ifeq (_$(REL_MONI)_,__)
    REL_MONI = $(REL_INC)/tools/moni08
endif

ifeq (_$(MONI_EXE)_,__)
	MONI_EXE = $(REL_MONI)/built/$(UMA)/moni08
endif

REL_SUPPL = $(REL_MONI)/supplement/$(VARIANT)

$(REL_SUPPL)/hc908$(VARIANT)-bootloader.bin:
	( env -i bash -l -c "$(MAKE) -C $(REL_SUPPL)" )
#	"env -i" ignores the old environment and any variables that might be already set by "make", "bash -l" creates a fresh environment with PATH etc.

$(REL_SUPPL)/hc908$(VARIANT)-bulkErase.bin:
	( env -i bash -l -c "$(MAKE) -C $(REL_SUPPL)" )

$(REL_MONI)/built/$(UMA)/moni08:
	( env bash -l -c "$(MAKE) -C $(REL_MONI)" )


MONI_08 = $(MONI_EXE) -v -b $(BAUD) -d $(TTY)

BULK_ERASE_LOADER = $(CPU_VARIANT)-bulkErase.bin

# ERASE_ARGS = -P $(POWER_ON_GAP) -a $(RAM_START) -l $(BULK_ERASE_LOADER) --exec -g $(ERASE_READY_GAP) -R -m $(FLASH_START)
FLASH_ARGS = -f $(TARGET)-final.s19 

BULK_ERASE_ARGS = -a $(NATIVE_RAM_START) -l $(BULK_ERASE_LOADER) --exec -g $(ERASE_READY_GAP) -m $(FLASH_START)
FW_ERASE_ARGS = -Z -g $(ERASE_READY_GAP) -m $(FLASH_START)

# F_PROG_ARGS = -P $(POWER_ON_GAP) $(FW_ERASE_ARGS) $(FLASH_ARGS)
F_PROG_ARGS = -P $(POWER_ON_GAP) $(BULK_ERASE_ARGS) $(FLASH_ARGS)


flash: $(CPU_VARIANT)-bootloader.bin $(TARGET)-final.s19 $(MONI_EXE) .moni08rc
	$(MONI_08) $(MONI08_RESET_OPT) -m $(FLASH_START) $(FLASH_ARGS) -m $(FLASH_START)

erase: $(BULK_ERASE_LOADER) $(MONI_EXE) .moni08rc
	$(MONI_08) -k -1 -P $(POWER_ON_GAP) $(BULK_ERASE_ARGS)

reflash: $(CPU_VARIANT)-bootloader.bin $(TARGET)-final.s19 $(MONI_EXE)  .moni08rc
	$(MONI_08) $(F_PROG_ARGS)

run: $(TARGET).bin $(MONI_EXE) .moni08rc
	$(MONI_08) $(MONI08_RESET_OPT) -a $(NATIVE_RAM_START) -l $(TARGET).bin --exec --tty

hc908$(VARIANT)-bootloader.bin: $(REL_SUPPL)/hc908$(VARIANT)-bootloader.bin
	ln -s $(REL_SUPPL)/hc908$(VARIANT)-bootloader.bin

hc908$(VARIANT)-bulkErase.bin: $(REL_SUPPL)/hc908$(VARIANT)-bulkErase.bin
	ln -s $(REL_SUPPL)/hc908$(VARIANT)-bulkErase.bin

.moni08rc: $(MONI_EXE)
	$(MONI_08) -v -b $(BAUD) -d $(TTY) -M $(MHZ) -c $(CPU_VARIANT) --saverc

readback:
	$(MONI_08) -k -1 -P 5000 -m 0xDC00 -n 8182 -r flash-readback.bin


