
ifeq (_$(BOARD)_,__)
    BOARD = crouzet_xt20_m2
endif

ifeq (_$(VARIANT)_,__)
    VARIANT = mr32
endif

ifeq (_$(CPU)_,__)
    CPU = mc68hc908$(VARIANT)
endif

ifeq (_$(REL_INC)_,__)
    REL_INC = ../../../..
endif

.CPU:
	cat $(REL_INC)/include/.$(CPU).mk >.CPU

$(CPU).asm:
	ln -s $(REL_INC)/include/$(CPU).asm .

reg68hc908$(VARIANT).asm:
	ln -s $(REL_INC)/include/reg68hc908$(VARIANT).asm .

bits68hc908.asm:
	ln -s $(REL_INC)/include/bits68hc908.asm .

macros.asm:
	ln -s $(REL_INC)/include/macros.asm .

fancy_macros.asm:
	ln -s $(REL_INC)/include/fancy_macros.asm .

