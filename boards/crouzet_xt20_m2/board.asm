;
;	specific equations for the "Crouzet-XT20 Millenium-II" board
;

		.INCLUDE        mc68hc908mr32.asm


		.IF	(START == RAM_START)
; probably running rom RAM in monitor mode
F_CPU	.EQU	1228750

		.ELSE

; running from FLASH in user mode, we can tweak something here with PLL clock
	.IF USE_PLL_CLOCK > 0
F_CPU	.EQU	7372500
	.ELSE
F_CPU	.EQU	1228750
	.ENDIF

		.ENDIF

		.IF PLATFORM > 0
			.ECHO "SCI functions from bootloader available"
; update Routines from HCS Bootloader
SciInit				.EQU	0xFCAD
ReadSci				.EQU	0xFD0B
WriteSci			.EQU	0xFD11
WriteSciLoop		.EQU	0xFCEF		 ; Start address in HX, length in LEN
WriteSciLen			.EQU	0x0063

		.ENDIF





