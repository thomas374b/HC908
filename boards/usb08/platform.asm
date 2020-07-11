;/*
; * platform.asm  of  usb08-board  for hc08sprg bootloader
; *
; *  Created on: 15.06.2020
; *      Author: pantec
; */

			.PROCESSOR		68908

		#include	"macros.asm"

		#include	"reg68hc908jb8.asm"

		.macro idents_
                .byte     "JB8",0    	; soft SCI
        .endm

; J2_P8  / PB1  ist  TxD
; J2_P10 / PB0  ist  RxD

TXDPORT         .EQU     PTA           ; <<<
TXDPIN          .EQU     0             ; <<<
SCITXINV        .EQU     0             ; (1 if SCI TX is inverted (no drivers))
SCIRXINV        .EQU     SCITXINV      ; (1 if SCI RX is inverted (no drivers))
RXDISIRQ        .EQU     0             ; (if RXD uses IRQ pin instead)

              #if (RXDISIRQ == 0)      ; RXDPORT & RXDPIN is defined
RXDPORT         .EQU     PTA           ; <<<
RXDPIN          .EQU     0             ; <<<
RXDPUEN         .EQU     1             ; use pull-up feature
			#else
RXDPUEN         .EQU     0             ; undefine pull-up enabling port
              #endif

BUSCLOCK        .EQU     3000000       ; <<< MODIFY SO IT IS NEAR YOUR BUS CLOCK!
SCISPEED        .EQU     9600

CALENABLED      .EQU     1             ; calibration enabled

              #if (CALENABLED == 0)
SPEED           .EQU     6             ; specify Xtal clk in MHz if no calibration (known freq.)
              #endif

CONFIG1			.EQU		CONFIG

	#ifndef RAM_START
RAM_START        	.EQU    	$40
	#endif
    
	#ifndef RAM_SIZE
RAM_SIZE		.EQU		256
	#endif

FLASH_START		.EQU	    $DC00       ; flash Start for Jx3

ROM_START     		.EQU    	$FC00		; Monitor

LOWEST_VECTOR_ADDR   	.EQU		$FFF0

;  TODO: calculate bootloader size, subtract from highest flash address
;	#ifndef FLASH_END
;FLASH_END		.EQU	 	$FA00       ; this is APL_VECT address (also from PRM file)
;	#endif

FLBPRMASK       	.EQU     	$E000       ; this is CPU specific FLBPR mask (i.e. bits that are always in the address)

ERBLK_LEN	  		.EQU	 512
WRBLK_LEN	    	.EQU	 64

;
;		.macro	dbnz
;				.byte		$3B,{1},(. - {2})
;		.endm
;
