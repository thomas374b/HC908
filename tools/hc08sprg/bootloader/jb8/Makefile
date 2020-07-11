
BOARD = usb08
VARIANT = jb8
BAUD = 9600
COLS = 32
TARGET = slfprgJB8

# end of application flash, start of bootloader
FLASH_END = 0xF9E0

# VECTOR_MODE_ARG = 65536 - FLASH_END - TABLE_SIZE - COLS
VECTOR_MODE = -z 384

include common.mk

common.mk:
	ln -s ../common.mk

