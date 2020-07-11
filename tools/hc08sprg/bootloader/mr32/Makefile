
BOARD = crouzet_xt20_m2
VARIANT = mr32
BAUD = 4800
COLS = 64
TARGET = slfprgMR32

# end of application flash, start of bootloader
FLASH_END = 0xFC00

# VECTOR_MODE_ARG = 65536 - FLASH_END - TABLE_SIZE - COLS
VECTOR_MODE = -z 0x3B2

include common.mk

common.mk:
	ln -s ../common.mk

