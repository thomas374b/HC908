Simple project that serves as a testing ground for the toolchain

The program toggles all LEDs on the usb08 evaluation board.

It is run from RAM in monitor-mode

To execute put your target device into monitor-mode (wiring and reset)
and type

    make run




To clean the directory type

    make distclean



To try flashing it (not yet working for me, hanging with the secondary loader)
    - change the Makefile
	START = $(NATIVE_RAM_START)
	# START = $(FLASH_START)
     to become
        # START = $(NATIVE_RAM_START)
        START = $(FLASH_START)

    - type
	make clean
	make
	make flash




