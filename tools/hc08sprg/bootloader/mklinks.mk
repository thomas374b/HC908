
include ../../../moni08/supplement/.mklinks.mk

platform.asm:
	ln -sf $(REL_INC)/boards/$(BOARD)/platform.asm .

board.asm:
	ln -sf $(REL_INC)/boards/$(BOARD)/board.asm .

