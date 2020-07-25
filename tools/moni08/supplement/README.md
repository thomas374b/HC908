Here you find bootloader assembly source code adapted to my hc908 boards
and written for my version of DASM.

The code contains supplementary helper routines for the moni08 flash program. 
Those files are downloaded to RAM in monitor mode and executed. They have to
be available in the same subdirectory when flashing with the moni08 program.

Details can be found in the Freescale Application Note AN2317 (see /docs subdirectory)

Each subdirectory is dedicated to a specific CPU type and platform.
You may have to edit the file .CPU (included into Makefile)
and to edit or (re-)create the files board.asm
(included in bootloader asm source)


