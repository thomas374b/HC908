;
;
;   small program to load into RAM and delete the flash eeprom
;
;
	.TRACE


	.INCLUDE	board.asm
	.INCLUDE	macros.asm
	.INCLUDE	fancy_macros.asm


fwCtrlByte		.EQU		0x68
fwCpuSpeed		.EQU		0x69
fwLastAddr		.EQU		0x6A
fwDataBlock		.EQU		0x6C

; _ldLastAddr		.EQU	  RAM_START + $04
_ldProgStart	.EQU	  RAM_START + $06

	.ORG	RAM_START
;	.ORG 	0  ; !! ONLY for testing/viewing the generated image

	.DS		(_ldProgStart - .),0

	.ORG	_ldProgStart
	.DC.w	FLASH_START

	.ORG	fwCtrlByte
	.DC.b	0

	.ORG	fwCpuSpeed
	.DC.b	5

	.ORG	fwLastAddr
	.DC.w	(FLASH_START+64)

	.ORG	fwDataBlock
	.DS		64,$FF

;	.ORG	$D0

STACK_CPY:
	.DC.w
ADRS:
	.DC.w	FLASH_START
LEN:
	.DC.b 	0
POM:
	.DC.b	0

;	declare_delay 100us,1,80			; 248 cycles,
;	declare_delay 4ms,13,252			; 9848 cycles, 4.007 ms

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


	.INCLUDE monitor-excerpts.asm

	declare_delay 4ms,7,234				; 4928 cycles,
	declare_delay 100us,1,40				; 128 cycles,

	.ORG	$FE
StackPointer:
	.DC.w	MainStart

;**************************************************************
FlashErase:
			ldHX		#FLBPR			; need this address for mass erase
			jsr			BulkErase

			ldHX		#_vect_SCItransmit
			jsr			BulkErase

			ldHX		#FLASH_START
			jsr			BulkErase

			rts


BulkErase:
			store_reg	FLBPR,#$FF
			store_reg	FLCR,#[ERASE|MASS]
;			bra 		FlashBurnBasic
;PageErase:
;			store_reg	FLCR,#ERASE
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

;*******************************************************************************************

; review timings when Crystal changed
T1US	  	.EQU		1
T5US	  	.EQU		3
T10US	  	.EQU		5
T30US	  	.EQU		13

T1MS	  	.EQU		3
T2MS	  	.EQU		4
T4MS	  	.EQU		7
T10MS	  	.EQU		17

T100MS_      .EQU     	255

 ;*******************************************************************************************
		.MACRO	d_ms
  			ldA	{1}		  		; [2] ||
_L2_{2}:	clrX		  		; [1] ||
_L1_{2}:
			dbnzX	_L1_{2}		; [3] |    256*3 = 768T
  			dbnzA	_L2_{2}		; [3] || (768+3)*(arg-1) + 2 T
  		.ENDM

		.MACRO d_us
  			ldA	{1}				; [2]
_L1_{2}:
  			dbnzA	_L1_{2}		; [3] 3*(arg-1) + 2 T
  		.ENDM

;*******************************************************************************************
FlashProg:
WR_ALG:
	; save content of HX which is our argument
			stHX	ADRS
	; calculate LEN from End-StartAddr
			clrA
			incA
addUp:
			incA
			incX
			cpX		fwLastAddr+1	; compare only lower byte
			bne		addUp
			stA		LEN

	; save stack register
			tSHX
			stHX	STACK_CPY

	; reload HX
			ldHX	ADRS

        	ldA     #%00000001
        	stA     FLCR            ; PGM bit on
        	ldA     FLBPR           ; dummy read FLBPR

        	stA     ,X              ; and write to desired range

        	d_us	#T10US, w10us	; wait 10us

        	ldA     #%00001001
    		stA		FLCR	        ; set HVEN, keep PGM

    		d_us	#T5US, w5us1	; wait 5us

        	ldHX	#fwDataBlock    ; prepare addrs'

    		tHXS
    		ldHX	ADRS
    		mov		LEN,POM
WR_ALG_L1:
    		pulA
    		stA		,X
    		aiX		#1
    		d_us	#T30US, w30us	; wait 30us
    		dbnz	POM,WR_ALG_L1   ; copy desired block of data

        	ldA     #%00001000
    		stA		FLCR	        ; keep HVEN, PGM off
    		d_us	#T5US, w5us2	; wait 5us

        	clrA
    		stA		FLCR    		; HVEN off
    		d_us	#T1US, w1us		; wait 1us

	; restore stack
			ldHX	STACK_CPY
			tHXS

			rts
WR_ALG_END:

