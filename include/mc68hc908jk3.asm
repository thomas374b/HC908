
		.PROCESSOR	68908

;
; variant specific defines
;

		#ifndef RAM_START
RAM_START       .EQU            $80
		#endif
		#ifndef RAM_SIZE
RAM_SIZE       .EQU             128
		#endif

		#ifndef FLASH_START
FLASH_START		.EQU            $EC00
		#endif
		#ifndef FLASH_SIZE
FLASH_SIZE		.EQU            4096
		#endif

	#include	"reg68hc908jk3.asm"
;
;  Register Section Bits FLCR
;
PGM			.EQU		$01
ERASE		.EQU		$02
MASS		.EQU		$04
HVEN		.EQU		$08

;MASS_ERASE	.EQU		(MASS|ERASE)
;NOT_ERASE	.EQU		((~ERASE) & $FF)
;NOT_HVEN	.EQU		(~HVEN)

;
; Firmware Monitor ROM specific section
;
ReadSerial	     	.EQU	$FC00
FlashRead       	.EQU    $FC03
FlashErase      	.EQU	$FC06
;FlashProgram    	.EQU	$FC09
FlashProg	    	.EQU	$FC77
DelayUs         	.EQU	$FC0C
WriteSerial	     	.EQU	$FED0
MoniRomDelayLoop	.EQU	$FD21


; cycles = (((A-3)*3 +10)*X) + 7
; longest wait time 25.1 s

fwCtrlByte      .EQU      RAM_START + $08
fwCpuSpeed      .EQU      RAM_START + $09
fwLastAddr      .EQU      RAM_START + $0A
fwDataBlock     .EQU      RAM_START + $0C


	#include	"macros.asm"

	.macro	fw_delay
		ldX		#{1}					;[2]
		ldA		#{2}					;[2]
		jsr		MoniRomDelayLoop		;[5]
	.endm

	.macro	declare_delay
Delay{1}:
		fw_delay {2},{3}
		rts								;[4]
	.endm

	; macro-names MUST be all lowercase
	.macro	fw_flash_erase_proc
		store_reg	fwCtrlByte,#$40
		store_reg	fwCpuSpeed,#(4*F_CPU)
		ldHX		#FLASH_START
		jsr			FlashErase
	.endm

	.macro	delay100us
		fw_delay	1,64		; ((((64-3)*3 +10)*1)+7) := 100 µs
	.endm

;	.IF (F_CPU > 1900000)
;		.macro	delay100us
;			fw_delay	1,64		; ((((64-3)*3 +10)*1)+7) := 100 µs
;		.endm
;	.ELSE
;	#endif
;

; ((((4-3)*3 +10)*1)+7) := 20 ticks @ 2MHz := 10 µs
;	.macro	delay10us
;		ldx		#1						;[2]
;		lda		#4						;[2]
;		jsr		MoniRomDelayLoop		;[5]
;	.endm

; ((((222-3)*3 +10)*6)+7) := 4.002 ms
;	.macro	delay4ms
;		ldx		#7
;		lda		#242
;		jsr		MoniRomDelayLoop
;	.endm
;
;	.ELSE
;
;	.macro	delay10us
;		bsr		Delay10us
;	.endm

;
;	.macro	delay100us
;		bsr		Delay100us
;	.endm
;
;	.macro	delay4ms
;		bsr		Delay4ms
;	.endm
;
;
;	#endif


