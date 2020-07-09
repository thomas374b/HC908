
ifeq (_$(BOARD)_,__)
    BOARD = relay7
endif

ifeq (_$(VARIANT)_,__)
    VARIANT = jk3
endif

ifeq (_$(CPU)_,__)
    CPU = mc68hc908$(VARIANT)
endif

REL_INC = ../..

# project defaults for target
.CPU:
	ln -s $(REL_INC)/include/.$(CPU).mk .CPU

# generic sources
timer_hc908.h:
	ln -s $(REL_INC)/generic/$@

adc_hc908.h:
	ln -s $(REL_INC)/generic/$@

types_hc908.h:
	ln -s $(REL_INC)/generic/$@

dumb_uart.c:
	ln -s $(REL_INC)/generic/$@

pintoggle.h:
	ln -s $(REL_INC)/generic/$@

nakedISR_hc908.h:
	ln -s $(REL_INC)/generic/$@

project.h:
	ln -s $(REL_INC)/generic/$@

token_pasting.h:
	ln -s $(REL_INC)/generic/$@

# SDCC scripts
sdcc.mk:
	ln -s $(REL_INC)/tools/sdcc/$@

s19.mk:
	ln -s $(REL_INC)/tools/sdcc/$@

# monitor mode flashing
moni08.mk:
	ln -s  $(REL_INC)/tools/moni08/contrib/flash.mk moni08.mk

# supplement.mk:
# 	ln -s ../$@

# hc08sprg flashing




