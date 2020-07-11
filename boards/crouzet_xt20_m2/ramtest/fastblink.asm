;/*
; * fastblink.asm
; *
; *  Created on: 11.06.2020
; *      Author: pantec
; */

USE_PLL_CLOCK 	.EQU	1	; 7.37MHz  Yesss

	#include	"board.asm"

next_var	.set	0x80

	declare_var		delayWait,2
	declare_var		portStrLen,1
	declare_var		portStr,40


; EQU and Labels _must_ start @ begin of line
OCR1A_MAX_VAL		.EQU	32768
TIMER_WRAPS			.EQU	RAM_START
PIN_FLAGS			.EQU	RAM_START+1
;_led_enable_mask	.EQU	(PTB6 | PTB7)

	#ifndef		START
START			.EQU	FLASH_START
	#endif

TARGET_IS_RAM	.EQU	(START < [RAM_START+RAM_SIZE])

	#if TARGET_IS_RAM
		.ORG	RAM_START
		.word	0

		.ORG	$FE
		.word	MainEntry
	#else
		.ORG	FLASH_START
	#endif


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

WritePorts:
	mov 	#0,portStrLen
	clrH

	; counter
	clrX
	ldA		TIMER_WRAPS
	bsr		MakePortHexNum

	ldA		#0x1C
	bsr		MakePortHexNum

	#if 1

	ldX		#PTA
	ldA		#4
PortReadA:
	bsr		ReadPort
	dbnzA	PortReadA
	ldX		#PTE
	ldA		#2
PortReadE:
	bsr		ReadPort
	dbnzA	PortReadE

	#endif

	; linefeed
	ldX		portStrLen
	ldA		#10
	stA		portStr,X
	incX
	inc		portStrLen

	mov		portStrLen, WriteSciLen
	ldHX	#portStr
	jsr		WriteSciLoop
	rts

WordDelay:			; max 26.4ms @ 7.3 MHz
	dbnzA	WordDelay			;	((3*A)+3)*X) +13
	dbnzX	WordDelay			;
	rts



MainEntry:
	ldA		#0xFF
	stA		COPCTL
    mov     #%10000001,CONFIG      ; COP disable

	ldHX	#[RAM_START+RAM_SIZE-1]
	tHXS

	clrX
	clrH
	stHX	delayWait

	#if USE_PLL_CLOCK > 0
		.echo "with higher CPU clock"
		; PLL clock test
;		 PPG := 0x66 after reset is what we want, 4.915 *6 / 4 := 7.3725 MHz
		switch_to_pll_clock
	#endif

	jsr		SciInit

	#if				TARGET_IS_RAM
	#else

	store_reg		CONFIG,#COPD	; COP disabled, COPRS long rate, LVID disabled

;					CONFIG2
;	stA				BRKH			; break register addres is 256 what should never match

	ldA				#0
;	stA				BRKSCR			; disable break interrupts
	stA				ADSCR			; disable ADC
;	stA				BRKL
;	stA				KBIER			; disable keyboard interrupts

	bset			1,ISCR			; ext IRQ disabled
;	bset			1,KBSCR			; keyboard IRQ disabled

	bclr			6,TASC			; input capture/output compare
	bclr			6,TBSC			; interrupts are disabled


	store_reg16		TBMODH, #OCR1A_MAX_VAL
	store_reg		TBSC, #$46		; TOIE & prescaler 1/64
	bclr			5,TBSC			; clear TSTOP	; clock enabled

	cli								; interrupts enabled

	store_reg		DDRF,#[[1<<0] | [1 << 2]]		; pins output

	#else

	mov			#[1<<4],DDRC

	#endif




	clrH

MainLoop:
	#if	TARGET_IS_RAM
		shake_pin	4,PTC,LED
	#else
;	stA		COPCTL
		shake_pin	0,PTF,LED
	#endif

	;ldHX	delayWait
;	pshH
;	pulA
;	incX
;	bcc		_noCarry
;	incA
;_noCarry:
;	pshA
;	pulA
;	stHX	delayWait

	ldHX	#0xF000
	jsr		WordDelay

	jsr		WritePorts

	bra MainLoop

	#if		TARGET_IS_RAM
	#else

TimerOvfIRQ:
	bclr		7,TBSC			; clear interrupt flag
	inc			TIMER_WRAPS
	shake_pin	2,PTF,CLK
DummyIRQ:						; re-use the byte for unused interrupts
	rti

Timer0IRQ:
	bclr		7,TBSC0		; interrupt is served
	rti

Timer1IRQ:
	bclr		7,TBSC1		; interrupt is served
	rti

ADCIRQ:
	ldA			ADRL				; interrupt is served
	rti

ExtnIRQ:
	bset		2,ISCR		; write ACKI, extern IRQ served
	rti


	declare_vector _vect_SCItransmit, 	 DummyIRQ
	declare_vector _vect_SCIreceive	, 	 DummyIRQ
	declare_vector _vect_SCIerror	, 	 DummyIRQ
	declare_vector _vect_SPItransmit, 	 DummyIRQ
	declare_vector _vect_SPIreceive	, 	 DummyIRQ
	declare_vector _vect_ADC		, ADCIRQ
	declare_vector _vect_TIMOVF		, TimerOvfIRQ
	declare_vector _vect_TIM1		, Timer1IRQ
	declare_vector _vect_TIM0		, Timer0IRQ
	declare_vector _vect_TimAOvf	, 	 DummyIRQ
	declare_vector _vect_TimACh3	, 	 DummyIRQ
	declare_vector _vect_TimACh2	, 	 DummyIRQ
	declare_vector _vect_TimACh1	, 	 DummyIRQ
	declare_vector _vect_TimACh0	, 	 DummyIRQ
	declare_vector _vect_PWMmc		, 	 DummyIRQ
	declare_vector _vect_Fault4		, 	 DummyIRQ
	declare_vector _vect_Fault3		, 	 DummyIRQ
	declare_vector _vect_Fault2		, 	 DummyIRQ
	declare_vector _vect_Fault1		, 	 DummyIRQ
	declare_vector _vect_PLL		, 	 DummyIRQ
	declare_vector _vect_IRQ		, ExtnIRQ
	declare_vector _vect_SWI		, 	 DummyIRQ
	declare_vector _vect_RESET		, MainEntry

	#endif



; #endif

