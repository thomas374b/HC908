;
;
;   small program to load into RAM and delete the flash eeprom
;
;

	#include	"board.asm"

; _ldLastAddr		.EQU	  RAM_START + $04
_ldProgStart	.EQU	  RAM_START + $06

	.ORG	RAM_START
	.DS		6,$DC

;	.ORG	_ldLastAddr
;	.word	$ECFF

	.ORG	_ldProgStart
	.word	FLASH_START

	.ORG	fwCtrlByte
;	.byte	(1 << 6)		; mass erase
	.byte	0

	.ORG	fwCpuSpeed
	.byte	8

	.ORG	fwLastAddr
	.word	(FLASH_START+64)

	.ORG	fwDataBlock
	.DS		64,$FF

;	.ORG	$D0

MainStart:
;	mov			#J2_P9,DDRD
MainLoop:
	ldHX	_ldProgStart
	jsr		FlashProg

	inc		fwLastAddr+1	; count the low-byte one up
	ldHX	_ldProgStart
tellResult:
	ldA		,X
	jsr 	WriteSerial
	incX
	cpX		fwLastAddr+1	; compare only lower byte
	bne		tellResult

	ldA		#"R"			; send ready signal
	jsr 	WriteSerial
	swi						; return to monitor code
	bra		MainLoop		; in case the monitor want us to run again

	.ORG	$FE
StackPointer:
	.word	MainStart


