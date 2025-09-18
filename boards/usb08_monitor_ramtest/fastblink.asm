;/*
; * fastblink.asm
; *
; *  Created on: 11.06.2020
; *      Author: pantec
; */




	#include	"board.asm"

;	#include	"macros.asm"

	#ifndef		START
START			.EQU	FLASH_START
	#endif

TARGET_IS_RAM	.EQU	(START < [RAM_START+RAM_SIZE])


; EQU and Labels _must_ start @ begin of line
OCR1A_MAX_VAL		.EQU	32768


next_var	.set	APPL_RAM_START

		declare_var		TIMER_WRAPS,1
		declare_var		delayWait,2
		declare_var		portStrLen,1
		declare_var		portStr,40		

	#if TARGET_IS_RAM
		.ORG	APPL_RAM_START
		.word	0					; TIMER_WRAPS
	#else
		.ORG	FLASH_START
	#endif



WordDelay:			
	dbnzX	WordDelay			;	((3*X)+3)*A) +13	= 2605
	dbnzA	WordDelay			;
	rts

	
Delay1ms:
	pshA
	pshX
	; exact 1ms mit 6MHz Quarz => 3MHz Bustakt
	ldX	#215
	ldA	#4
	bsr	WordDelay
	pulX
	pulA
	rts	


Delay250ms:
	pshA
	ldA	#250
d250loop:	
	bsr	Delay1ms
	dbnzA	d250loop			
	pulA
	rts	


LedCounter:
	inc		TIMER_WRAPS
	ldA		TIMER_WRAPS
	and		#7
	stA		PTD
	rts
	

MainEntry:
	ldHX	#[RAM_START+RAM_SIZE-1]
	tHXS
	
	
	#if TARGET_IS_RAM
	#else
;		ldA		#0xFF
;		stA		COPCTL
;	  mov     #%10000001,CONFIG      ; COP disable
	#endif


	ldA		#7
	stA		DDRD		; enable LED ports as output
  bset	0,DDRC
  bset	7,DDRA

	#if				TARGET_IS_RAM
	#else
	
;	clrX
;	clrH
;	stHX	delayWait

		store_reg		CONFIG,#COPD	; COP disabled, COPRS long rate, LVID disabled

;					CONFIG2
;		stA				BRKH			; break register addres is 256 what should never match

		ldA				#0
		stA				BRKSCR			; disable break interrupts
;	stA				ADSCR			; disable ADC
;	stA				BRKL
		stA				KBIER			; disable keyboard interrupts

		bset			1,ISCR			; ext IRQ disabled
		bset			1,KBSCR			; keyboard IRQ disabled

		bclr			6,TSC			; input capture/output compare interrupts are disabled
		store_reg16		TMODH, #OCR1A_MAX_VAL
		store_reg		TSC, #$46			; TOIE & prescaler 1/64
		bclr			5,TSC						; clear TSTOP	; clock enabled

;		cli								; interrupts enabled

;		store_reg		DDRF,#[[1<<0] | [1 << 2]]		; pins output

	#endif

;	clrH

MainLoop:
	shake_pin	0,PTC,LEDC
  
	#if	TARGET_IS_RAM
		shake_pin	7,PTA,LEDA		
	#else
;		ldHX	delayWait
	#endif

	bsr LedCounter
	
	bsr Delay250ms

	
;	pshA	
;	beq _skip
	
_skip:
;	pulA
	
;	stA		COPCTL
;	pshH
;	pulA
;	incX
;	bcc		_noCarry
;	incA
;_noCarry:
;	pshA
;	pulA
;	stHX	delayWait

	bra MainLoop



    #if TARGET_IS_RAM

	    .ORG		0xFE
	    .word		MainEntry
	    .ORG		[RAM_START+RAM_SIZE-2]

    #else

MakePortHexNum:			; X must point to string end in RAM variable, A has value to be converted
	pshA
	ldA		#"x"
	stA		portStr,X
	incX
	inc		portStrLen
	pulA
	pshA
	lsrA
	lsrA
	lsrA
	lsrA
	and		#0x0F
	cmp		#9
	bls 	isNumber0
	add		#0x37
	bra 	nextChar0
isNumber0:
	orA		#0x30
nextChar0:
	stA		portStr,X
	incX
	inc		portStrLen
	pulA
	and		#0x0F
	cmp		#9
	bhi		isLetter1
	orA		#0x30
	bra 	nextChar1
isLetter1:
	add		#0x37
nextChar1:
	stA		portStr,X
	incX
	inc		portStrLen
	ldA		#","
	stA		portStr,X
	incX
	inc		portStrLen
	rts

ReadPort:				; X has port address
	pshA
	ldA		,X
	pshX
	ldX		portStrLen
	bsr		MakePortHexNum
	pulX
	incX
	pulA
	rts

TimerOvfIRQ:
	bclr		7,TSC			; clear interrupt flag
	jsr		LedCounter
	shake_pin	7,PTA,CLK
DummyIRQ:					; re-use the byte for unused interrupts
	rti

Timer0IRQ:
	bclr		7,TSC0		; interrupt is served
	rti

Timer1IRQ:
	bclr		7,TSC1		; interrupt is served
	rti

ExtnIRQ:
	bset		2,ISCR		; write ACKI, extern IRQ served
	rti

KeybIRQ:
	bset		2,KBSCR
	rti

     .ORG            LOWEST_VECTOR_ADDR

	declare_vector _vect_KBIR		, KeybIRQ
	declare_vector _vect_TIMOVF		, TimerOvfIRQ
	declare_vector _vect_TIM1		, Timer1IRQ
	declare_vector _vect_TIM0		, Timer0IRQ
	declare_vector _vect_IRQ		, ExtnIRQ
	declare_vector _vect_SWI		, DummyIRQ
	declare_vector _vect_RESET		, MainEntry

    #endif


; #endif

