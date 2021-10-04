
ifeq (_$(BOARD)_,__)
    BOARD = crouzet_xt20_m2
endif

ifeq (_$(VARIANT)_,__)
    VARIANT = mr32
endif

ifeq (_$(CPU)_,__)
    CPU = mc68hc908$(VARIANT)
endif

REL_INC = ../../..

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

spi_hc908.h:
	ln -s $(REL_INC)/generic/$@

keyMatrix.h:
	ln -s $(REL_INC)/generic/$@

HD44780_interface.h:
	ln -s $(REL_INC)/generic/$@

pintoggle.h:
	ln -s $(REL_INC)/generic/$@

truly_mcc315.h:
	ln -s $(REL_INC)/generic/$@

truly_mcc315.c:
	ln -s $(REL_INC)/generic/$@

bit_reverse.h:
	ln -s $(REL_INC)/generic/$@

project.h:
	ln -s $(REL_INC)/generic/$@

token_pasting.h:
	ln -s $(REL_INC)/generic/$@

i2c_hc908.h:
	ln -s $(REL_INC)/generic/$@

uart_hc908.h:
	ln -s $(REL_INC)/generic/$@

m41t56.h:
	ln -s $(REL_INC)/generic/$@

sliding_int_avg.c:
	ln -s $(REL_INC)/generic/$@

sliding_int_avg.h:
	ln -s $(REL_INC)/generic/$@

uart_printd.h:
	ln -s $(REL_INC)/generic/$@

uart_printb.h:
	ln -s $(REL_INC)/generic/$@

uart_printh.h:
	ln -s $(REL_INC)/generic/$@

printd_type.h:
	ln -s $(REL_INC)/generic/$@

globalVarSize.h:
	ln -s $(REL_INC)/generic/$@


board.h:
	ln -s ../$@

peripherie.h:
	ln -s ../$@

config.h:
	ln -s ../$@


# SDCC scripts
sdcc.mk:
	ln -s $(REL_INC)/tools/sdcc/$@

s19.mk:
	ln -s $(REL_INC)/tools/sdcc/$@


# monitor mode flashing
moni08.mk:
	ln -s $(REL_INC)/tools/moni08/contrib/flash.mk moni08.mk

# hc08sprg flashing
flash.mk:
	ln -s $(REL_INC)/tools/hc08sprg/contrib/flash.mk



