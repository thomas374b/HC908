
REL_INC = ../../../..

.CPU:
	ln -s $(REL_INC)/include/.MC68HC908JB8 .CPU

mc68hc908jb8.asm:
	ln -s $(REL_INC)/include/mc68hc908jb8.asm .

reg68hc908jb8.asm:
	ln -s $(REL_INC)/include/reg68hc908jb8.asm .

bits68hc908.asm:
	ln -s $(REL_INC)/include/bits68hc908.asm .

macros.asm:
	ln -s $(REL_INC)/include/macros.asm .

fancy_macros.asm:
	ln -s $(REL_INC)/include/fancy_macros.asm .

board.asm:
	ln -s $(REL_INC)/boards/usb08/board.asm .

