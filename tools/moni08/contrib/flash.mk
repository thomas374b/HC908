

ifeq (_$(BAUD)_,__)
	BAUD = 7680
endif

ifeq (_$(TTY)_,__)
	TTY = /dev/ttyS0
endif


MONI_EXE = ../../../moni08/built/Linux-i686/moni08
MONI_08 = $(MONI_EXE) -v -b $(BAUD) -d $(TTY)

POWER_ON_GAP = 5000
ERASE_READY_GAP = 1000
BULK_ERASE_LOADER = $(CPU_VARIANT)-bulkErase.bin

# ERASE_ARGS = -P $(POWER_ON_GAP) -a $(RAM_START) -l $(BULK_ERASE_LOADER) --exec -g $(ERASE_READY_GAP) -R -m $(FLASH_START)
FLASH_ARGS = -f $(TARGET)-final.s19 

BULK_ERASE_ARGS = -a $(RAM_START) -l $(BULK_ERASE_LOADER) --exec -g $(ERASE_READY_GAP) -R -m $(FLASH_START)
FW_ERASE_ARGS = -Z -g $(ERASE_READY_GAP) -m $(FLASH_START)

# F_PROG_ARGS = -P $(POWER_ON_GAP) $(FW_ERASE_ARGS) $(FLASH_ARGS)
F_PROG_ARGS = -P $(POWER_ON_GAP) $(BULK_ERASE_ARGS) $(FLASH_ARGS)



flash: $(CPU_VARIANT)-bootloader.bin $(TARGET)-final.s19
	$(MONI_08) -R -m $(FLASH_START) $(FLASH_ARGS) -m $(FLASH_START)

erase: $(BULK_ERASE_LOADER)
	$(MONI_08) -k -1 -P $(POWER_ON_GAP) $(BULK_ERASE_ARGS)

reflash: $(CPU_VARIANT)-bootloader.bin $(TARGET)-final.s19
	$(MONI_08) $(F_PROG_ARGS)

