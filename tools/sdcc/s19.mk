

ifeq (_$(COLS)_,__)
	COLS = 32
endif


$(TARGET)-final.s19: $(TARGET)-$(FLASH_START).bin
	@Bin2S19 -c $(COLS) -b $(FLASH_START) -f $(TARGET)-$(FLASH_START).bin -z $(FLASH_SIZE) -s >$(TARGET)-final.s19
	@Bin2S19 -c 1 -b $(FLASH_START) -f $(TARGET)-$(FLASH_START).bin -s >/dev/null

$(TARGET)-$(FLASH_START).bin: $(TARGET).s19
	Bin2S19 -w -f $(TARGET)

show_size: $(TARGET)-$(FLASH_START).bin
	Bin2S19 -c 1 -b $(FLASH_START) -f $(TARGET)-$(FLASH_START).bin -s >/dev/null