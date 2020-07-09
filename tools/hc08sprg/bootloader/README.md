Here you find bootloader assembly source code adapted to my hc908 boards
and written for my version of DASM.

The bootloader code allows in-system flashing via serial line. It has to be 
flashed to the target CPU before the application. Once it is installed you 
simply wire the serial line, start the hc08sprg flash-program and power-on 
the target.

The bootloader tries to re-use code from monitor-ROM as much as possible 
and consumes only few bytes.

Details can be found in the Freescale Application Note XXXX (Todo:)

Each subdirectory is dedicated to a specific CPU type and platform.
You may have to edit the file .CPU (included into Makefile)
and to edit or (re-)create the files *-platform.asm, board.asm
(included in bootloader asm source)


