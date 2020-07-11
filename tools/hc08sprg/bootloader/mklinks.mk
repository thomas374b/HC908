
include ../../../moni08/supplement/.mklinks.mk

platform.asm:
	ln -sf $(REL_INC)/boards/$(BOARD)/platform.asm .

board.asm:
	ln -sf $(REL_INC)/boards/$(BOARD)/board.asm .

flash.mk:
	ln -sf ../../../moni08/contrib/flash.mk

