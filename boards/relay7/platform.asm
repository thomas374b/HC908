;/*
; * relais7-platform.asm for hc08sprg bootloader
; *
; *  Created on: 15.06.2020
; *      Author: pantec
; */

			.PROCESSOR		68908


		#include	"macros.asm"

		#include	"reg68hc908jk3.asm"

		.macro idents_
                .byte     "JK3",0    	; soft SCI
        .endm

; J2_P8  / PB1  ist  TxD
; J2_P10 / PB0  ist  RxD

TXDPORT         .EQU     PTB           ; <<<
TXDPIN          .EQU     1             ; <<<
SCITXINV        .EQU     0             ; (1 if SCI TX is inverted (no drivers))
SCIRXINV        .EQU     SCITXINV      ; (1 if SCI RX is inverted (no drivers))
RXDISIRQ        .EQU     0             ; (if RXD uses IRQ pin instead)

			#if (RXDISIRQ == 0)         ; RXDPORT & RXDPIN is defined
RXDPORT         .EQU     PTB           ; <<<
RXDPIN          .EQU     0             ; <<<
RXDPUEN         .EQU     1             ; use pull-up feature
			#else
RXDPUEN         .EQU     0             ; undefine pull-up enabling port
			#endif

BUSCLOCK        .EQU     1003422       ; <<< MODIFY SO IT IS NEAR YOUR BUS CLOCK!
SCISPEED        .EQU     9600

CALENABLED      .EQU     1             ; calibration enabled

			#if (CALENABLED == 0)
SPEED           .EQU     4             ; specify Xtal clk in MHz if no calibration (known freq.)
			#endif

CONFIG1					.EQU		CONFIG

RAM_START        		.EQU    	$80

FLASH_START		  		.EQU	    $EC00       ; flash Start for Jx3

ROM_START     			.EQU    	$FC00

LOWEST_VECTOR_ADDR   	.EQU		$FFDE

;  TODO: calculate bootloader size, subtract from highest flash address
FLASH_END				.EQU	 	$FA80       ; this is APL_VECT address (also from PRM file)

FLBPRMASK       		.EQU     	$E000       ; this is CPU specific FLBPR mask (i.e. bits that are always in the address)


;
;		.macro	dbnz
;				.DC.b		$3B,{1},(. - {2})
;		.endm
;
