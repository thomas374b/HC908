
BOARD = relay7
VARIANT = jk3
BAUD = 7680
COLS = 16
TARGET = slfprgJK3

# end of application flash, start of bootloader
FLASH_END = 0xFA80

# VECTOR_MODE_ARG = 65536 - FLASH_END - TABLE_SIZE - COLS
# VECTOR_MODE = -z 384

include common.mk

common.mk:
	ln -s ../common.mk
