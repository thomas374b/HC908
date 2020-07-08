
		.PROCESSOR	68908

;
; variant specific defines
;

		.IFNCONST RAM_START
RAM_START       .EQU            $40
		.ENDIF
		.IFNCONST RAM_SIZE
RAM_SIZE       .EQU             256
		.ENDIF

		.IFNCONST FLASH_START
FLASH_START		.EQU            $DC00
		.ENDIF
		.IFNCONST FLASH_SIZE
FLASH_SIZE		.EQU            8192
		.ENDIF

	.INCLUDE	reg68hc908jb8.asm



PTE0	.EQU	(1 << 0)
PTE1	.EQU	(1 << 1)
PTE2	.EQU	(1 << 2)
PTE3	.EQU	(1 << 3)
PTE4	.EQU	(1 << 4)
PTE5	.EQU	(1 << 5)
PTE6	.EQU	(1 << 6)
PTE7	.EQU	(1 << 7)


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
FlashProg	    	.EQU	$FC77	; $FC09
DelayUs         	.EQU	$FC0C	;

WriteSerial	     	.EQU	$FED6
MoniRomDelayLoop	.EQU	$FD21		; cycles = (((A-3)*3 +10)*X) + 7
										; longest wait time 58.6ms

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

	.MACRO	delay5us
		fw_delay	1,3			; ((((3-3)*3 +10)*1)+7) := 17 cycles  ~5.6 �s @ 3MHz
	.ENDM

	.MACRO	delay10us
		fw_delay	1,8			; ((((8-3)*3 +10)*1)+7) := 32 cycles  ~10�s @ 3MHz
	.ENDM

	.MACRO	delay100us
		fw_delay	1,98		; ((((98-3)*3 +10)*1)+7) := 302 cycles  100�s @ 3MHz
	.ENDM

	.MACRO	delay2ms
		fw_delay	8,250		; ((((250-3)*3 +10)*1)+7) := 6015 cycles  2ms @ 3MHz
	.ENDM

	.MACRO	delay4ms
		fw_delay	16,250		; ((((250-3)*3 +10)*16)+7) := 12023 cycles should make 4ms @ 3MHz
	.ENDM

