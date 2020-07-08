
REL_INC = ../../../..

.CPU:
	ln -s $(REL_INC)/include/.MC68HC908JK3 .CPU

mc68hc908jk3.asm:
	ln -s $(REL_INC)/include/mc68hc908jk3.asm .

reg68hc908jk1.asm:
	ln -s $(REL_INC)/include/reg68hc908jk1.asm .

bits68hc908.asm:
	ln -s $(REL_INC)/include/bits68hc908.asm .

macros.asm:
	ln -s $(REL_INC)/include/macros.asm .

fancy_macros.asm:
	ln -s $(REL_INC)/include/fancy_macros.asm .

board.asm:
	ln -s $(REL_INC)/boards/relay7/board.asm .

