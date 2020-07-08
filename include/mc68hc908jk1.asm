
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
FLASH_START		.EQU            $F600
		.ENDIF
		.IFNCONST FLASH_SIZE
FLASH_SIZE		.EQU            1536
		.ENDIF

	.INCLUDE	reg68hc908jk1.asm
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

ReadMonitor     	.EQU	$FC00
FlashRead       	.EQU    $FC03
FlashErase      	.EQU	$FC06
FlashProgram    	.EQU	$FC09
DelayUs         	.EQU	$FC0C




MoniRomDelayLoop	.EQU		$FD21
; cycles = (((A-3)*3 +10)*X) + 7
; longest wait time 25.1 s

fwCtrlByte      .EQU      RAM_START + $08
fwCpuSpeed      .EQU      RAM_START + $09
fwLastAddr      .EQU      RAM_START + $0A
fwDataBlock     .EQU      RAM_START + $0C


	.INCLUDE	macros.asm


	.MACRO	declare_delay
Delay{1}:
		ldx		#{2}					;[2]
		lda		#{3}					;[2]
		jsr		MoniRomDelayLoop		;[5]
		rts								;[4]
	.ENDM


	.MACRO	fwFlashEraseProc
		store_reg	fwCtrlByte,#$40
		store_reg	fwCpuSpeed,#(4*F_CPU)
		ldhx		#$DC00
		jsr			FlashErase
	.ENDM




;	.IF (F_CPU > 1900000)

; ((((4-3)*3 +10)*1)+7) := 20 ticks @ 2MHz := 10 µs
;	.MACRO	delay10us
;		ldx		#1						;[2]
;		lda		#4						;[2]
;		jsr		MoniRomDelayLoop		;[5]
;	.ENDM

; ((((64-3)*3 +10)*1)+7) := 100 µs
;	.MACRO	delay100us
;		ldx		#1
;		lda		#64
;		jsr		MoniRomDelayLoop
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


