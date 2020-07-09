;
; 
; when running on usb08 power supply in half-speed monitor-mode
;
;

	.INCLUDE	mc68hc908jk3.asm

	.IF	(START == RAM_START)
; probably running rom RAM in monitor mode
F_CPU	.EQU	2041322
	.ELSE
; running from FLASH in user mode
F_CPU	.EQU	1003422
	.ENDIF


J2_P1	.EQU	PTD5			; TCH1
J2_P2	.EQU	PTD4			; TCH0
;J2_P3	.EQU	VDD
;J2_P4	.EQU	EXT_IRQ		; pull down to IRQ
;J2_P5	.EQU	N_C_
J2_P6	.EQU	PTB2		; bootloader pull-down
J2_P7	.EQU	PTD2			; ADC9
J2_P8	.EQU	PTB1		; bootloader pull-up
J2_P9	.EQU	PTD3			; ADC8
J2_P10	.EQU	PTB0		; boot loader data-IO


RELAY0	.EQU	PTB3
RELAY1	.EQU	PTB4
RELAY2	.EQU	PTD6
RELAY3	.EQU	PTD7
RELAY4	.EQU	PTB5
RELAY5	.EQU	PTB6
RELAY6	.EQU	PTB7










