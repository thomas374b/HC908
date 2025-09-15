
include $(REL_INC)/tools/moni08/supplement/.mklinks.mk

moni08.mk:
	ln -sf $(REL_INC)/tools/moni08/contrib/flash.mk moni08.mk

PARENT = ../usb08

board.asm:
	ln -s $(PARENT)/$@

platform.asm:
	ln -s $(PARENT)/$@




