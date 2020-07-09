
		.PROCESSOR	68908

;
; variant specific defines
;

		.IFNCONST RAM_START
RAM_START       .EQU            $80
		.ENDIF
		.IFNCONST RAM_SIZE
RAM_SIZE       .EQU             128
		.ENDIF

		.IFNCONST FLASH_START
FLASH_START		.EQU            $EC00
		.ENDIF
		.IFNCONST FLASH_SIZE
FLASH_SIZE		.EQU            4096
		.ENDIF

	.INCLUDE	reg68hc908jk3.asm
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


	.INCLUDE	macros.asm

	.MACRO	fw_delay
		ldX		#{1}					;[2]
		ldA		#{2}					;[2]
		jsr		MoniRomDelayLoop		;[5]
	.ENDM

	.MACRO	declare_delay
Delay{1}:
		fw_delay {2},{3}
		rts								;[4]
	.ENDM

	; macro-names MUST be all lowercase
	.MACRO	fw_flash_erase_proc
		store_reg	fwCtrlByte,#$40
		store_reg	fwCpuSpeed,#(4*F_CPU)
		ldHX		#FLASH_START
		jsr			FlashErase
	.ENDM

		.MACRO	delay100us
			fw_delay	1,64		; ((((64-3)*3 +10)*1)+7) := 100 µs
		.ENDM

;	.IF (F_CPU > 1900000)
;		.MACRO	delay100us
;			fw_delay	1,64		; ((((64-3)*3 +10)*1)+7) := 100 µs
;		.ENDM
;	.ELSE
;	.ENDIF
;

; ((((4-3)*3 +10)*1)+7) := 20 ticks @ 2MHz := 10 µs
;	.MACRO	delay10us
;		ldx		#1						;[2]
;		lda		#4						;[2]
;		jsr		MoniRomDelayLoop		;[5]
;	.ENDM

; ((((222-3)*3 +10)*6)+7) := 4.002 ms
;	.MACRO	delay4ms
;		ldx		#7
;		lda		#242
;		jsr		MoniRomDelayLoop
;	.ENDM
;
;	.ELSE
;
;	.MACRO	delay10us
;		bsr		Delay10us
;	.ENDM

;	.MACRO	delay100us
;		bsr		Delay100us
;	.ENDM

;	.MACRO	delay4ms
;		bsr		Delay4ms
;	.ENDM
;
;
;	.ENDIF


