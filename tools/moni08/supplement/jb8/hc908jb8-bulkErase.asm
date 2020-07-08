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

	declare_delay 100us,1,98

MainStart:
	store_reg	FLBPR, #$FF		; disable flash protection
	store_reg	FLCR, #[MASS|ERASE]

;	store_reg	FLBPR, #$FF		;

	store_reg	$FFF0, #$0A		; need this address for mass erase
	ldA			#4
_shortDelay:
	dbnzA		_shortDelay		;	[3] *5  15

	set_bits	FLCR, HVEN

	delay2ms

	clear_bits	FLCR, ERASE

	bsr			Delay100us

	clear_bits	FLCR,HVEN

;	lda			#$EA
;	psha
;	jsr			WriteSerial
;	pula
;	jsr 		ReadSerial

	ldA		#"R"			; send ready signal
	jsr 	WriteSerial

; pieps wenn fertig
	store_reg	DDRD,#PTD3
ReadyLoop:						; [247]
	store_reg	PTD,#PTD3
 	bsr			Delay100us
 	store_reg	PTD,#0			; [2+2]
 	bsr			Delay100us				; [17+]
	bra 		ReadyLoop		; [3]

	.ORG		$FE
StackPointer:
	.DC.w		MainStart


