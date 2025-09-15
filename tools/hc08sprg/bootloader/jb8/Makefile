
BOARD = usb08
VARIANT = jb8
BAUD = 9600
COLS = 64
TARGET = slfprgJB8

# end of application flash, start of bootloader
FLASH_END = 0xFA00

# VECTOR_MODE_ARG = 65536 - FLASH_END - TABLE_SIZE - COLS
VECTOR_MODE = -z 1328
# cols 64 -z 1328
# cols 32 -z 1360
# cols 16 -z 1376


DEFINES += -DUSE_BOOTLOADER=0

include common.mk

common.mk:
	ln -s ../common.mk

