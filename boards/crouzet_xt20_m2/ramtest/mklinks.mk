
include $(REL_INC)/tools/moni08/supplement/.mklinks.mk

moni08.mk: .moni08rc
	ln -s $(REL_INC)/tools/moni08/contrib/flash.mk moni08.mk

flash.mk:
	ln -s $(REL_INC)/tools/hc08sprg/contrib/flash.mk

board.asm:
	ln -s ../$@

.moni08rc:
	ln -s ../$@





