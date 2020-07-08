;
;
;   small program to load into RAM and delete the flash eeprom
;
;
;	.TRACE


	.INCLUDE	board.asm
	.INCLUDE	macros.asm
	.INCLUDE	fancy_macros.asm

	.ORG		RAM_START

;	declare_delay 100us,1,80			; 248 cycles,
	declare_delay 100us,1,40				; 128 cycles,

;	declare_delay 4ms,13,252			; 9848 cycles, 4.007 ms
	declare_delay 4ms,7,234				; 4928 cycles,

	.INCLUDE monitor-excerpts.asm

MainStart:
;	store_reg	FLBPR, #$FF		; disable flash protection

	ldHX		#FLBPR			; need this address for mass erase
	jsr			BulkErase

	ldHX		#_vect_SCItransmit
	jsr			BulkErase

	ldHX		#FLASH_START
	jsr			BulkErase

;	ldA		#"R"			; send ready signal
;	jsr 	WriteSerial

	swi

; pieps wenn fertig
;	store_reg	DDRC,#PTC4
;ReadyLoop:						; [247]
;	bset		4,PTC
; 	bsr			Delay100us
;	bclr		4,PTC
; 	bsr			Delay100us				; [17+]
;	bra 		ReadyLoop		; [3]


;PageProgram:
;	store_reg	FLCR,#PGM
;	bra FlashBurnBasic

BulkErase:
	store_reg	FLBPR,#$FF
	store_reg	FLCR,#[ERASE|MASS]
	bra 		FlashBurnBasic
PageErase:
	store_reg	FLCR,#ERASE
FlashBurnBasic:
	ldA			FLBPR
	ldA			#$FF
	stA			,X

	ldA			#4
_shortDelay:
	dbnzA		_shortDelay		;	[3] *4   12 ; 10µs

	set_bits	FLCR, #HVEN

	bsr 		Delay4ms		; much too long for programming

	store_reg	FLCR, #HVEN		; clear all except HVEN

	bsr			Delay100us

	store_reg	FLCR, #0		; all bits cleared

	rts



	.ORG		$FE
StackPointer:
	.DC.w		MainStart






