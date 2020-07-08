;
;
;   small program to load into RAM and delete the flash eeprom   
;
;
	.TRACE


	.INCLUDE	board.asm
	.INCLUDE	macros.asm
	.INCLUDE	fancy_macros.asm


	.ORG		RAM_START
;	.ORG		$90

	.IF (F_CPU > 1900000)
		declare_delay 100us,1,64

; ((((8-3)*3 +10)*1)+7) := 32 ticks @ 2MHz := 16 µs
		.MACRO	delay10us
			ldX		#1						;[2]
			ldA		#8						;[2]
			jsr		MoniRomDelayLoop		;[5]
		.ENDM

; ((((64-3)*3 +10)*1)+7) := 100 µs
;	.MACRO	delay100us
;		ldx		#1
;		lda		#64
;		jsr		MoniRomDelayLoop
;	.ENDM

; ((((242-3)*3 +10)*11)+7) := 4.002 ms
		.MACRO	delay4ms
			ldX		#11
			ldA		#242
			jsr		MoniRomDelayLoop
		.ENDM

	.ELSE
;		declare_delay 10us,1,4
		declare_delay 100us,1,31
;		declare_delay 4ms,6,222
	.ENDIF



MainStart:

	store_reg	FLBPR, #$FF		; disable flash protection

	store_reg	FLCR, #[MASS|ERASE]
;	store_reg	FLBPR, #$FF		; disable flash protection
	store_reg	$FFF0, #$0A		; need this address for mass erase
	delay10us
	set_bits	FLCR, HVEN
	delay4ms
	clear_bits	FLCR, ERASE
	bsr	Delay100us
	clear_bits	FLCR,HVEN

;	lda			#$EA
;	psha
;	jsr			WriteSerial
;	pula
;	jsr 		ReadSerial

	ldA		#"R"			; send ready signal
	jsr 	WriteSerial

; pieps wenn fertig
	store_reg	DDRD,#J2_P9
ReadyLoop:						; [247]
	store_reg	PTD,#J2_P9
 	bsr	Delay100us
 	store_reg	PTD,#0			; [2+2]
 	bsr	Delay100us				; [17+]
	bra 		ReadyLoop		; [3]


; ((((4-3)*3 +10)*1)+7) := 20 ticks @ 2MHz := 10 µs
;    alignspace	t0,8
;Delay100us:
;	;delay100us
;	rts


	.ORG		$FE
StackPointer:
	.DC.w		MainStart


