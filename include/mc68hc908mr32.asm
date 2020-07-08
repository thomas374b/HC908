
		.PROCESSOR	68908

;
; variant specific defines
;

		.IFNCONST RAM_START
RAM_START       .EQU            $60
		.ENDIF

		.IFNCONST RAM_SIZE
RAM_SIZE       .EQU             768
		.ENDIF

		.IFNCONST FLASH_START
FLASH_START		.EQU            $8000
		.ENDIF

		.IFNCONST FLASH_SIZE
FLASH_SIZE		.EQU            32256
		.ENDIF

	.INCLUDE	reg68hc908mr32.asm


	.INCLUDE	macros.asm

	.MACRO	switch_to_pll_clock
		clr 	PCTL
		bset	5,PCTL	 		; switch PLL on
		bset	7,PBWC			; automatic bandwidth
PLL_lock_wait:
		brclr	6,PBWC,PLL_lock_wait
		bset	4,PCTL			; select PLL clock as system
	.ENDM


; ROM specific routines
WriteSerial		.EQU		0xFEC3

;	.MACRO	d_ms
;  			ldA	{1}		  		; [2] ||
;_L2_{2}:	clrX		  		; [1] ||
;_L1_{2}:
;			dbnzX	_L1_{2}		; [3] |    256*3 = 768T
;  			dbnzA	_L2_{2}		; [3] || (768+3)*(arg-1) + 2 T
;  	.ENDM

;	.MACRO d_us
;  			ldA	{1}				; [2]
;_L1_{2}:
;  			dbnzA	_L1_{2}		; [3] 3*(arg-1) + 2 T
;  	.ENDM

	.MACRO	fw_delay
		ldX		#{1}					;[2]
		ldA		#{2}					;[2]
		jsr		MoniRomDelayLoop		;[5]		// TODO: find this in the monitor code
	.ENDM

	.MACRO	declare_delay
Delay{1}:
		fw_delay {2},{3}
		rts								;[4]
	.ENDM

;	; macro-names MUST be all lowercase
;	.MACRO	fw_flash_erase_proc
;		store_reg	fwCtrlByte,#$40
;		store_reg	fwCpuSpeed,#(4*F_CPU)
;		ldHX		#FLASH_START
;		jsr			FlashErase
;	.ENDM

;		.MACRO	delay100us
;			fw_delay	1,64		; ((((64-3)*3 +10)*1)+7) := 100 µs
;		.ENDM

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


