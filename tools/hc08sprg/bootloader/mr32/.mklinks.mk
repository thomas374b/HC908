
include ../../../moni08/supplement/.mklinks.mk

platform.asm:
	ln -s $(REL_INC)/boards/$(BOARD)/platform.asm .

flash.mk:
	ln -s ../../../moni08/contrib/flash.mk

hc908$(VARIANT)-bootloader.bin:
	$(MAKE) -C ../../../moni08/supplement/$(VARIANT)
	ln -s ../../../moni08/supplement/$(VARIANT)/hc908$(VARIANT)-bootloader.bin .

hc908$(VARIANT)-bulkErase.bin:
	$(MAKE) -C ../../../moni08/supplement/$(VARIANT)
	ln -s ../../../moni08/supplement/$(VARIANT)/hc908$(VARIANT)-bulkErase.bin

../../../moni08/built/Linux-i686/moni08:
	$(MAKE) -C ../../../moni08