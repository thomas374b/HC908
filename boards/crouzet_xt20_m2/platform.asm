;/*
; *  platform.asm for the slfprg bootloader
; *
; *  Created on: 15.06.2020
; *      Author: pantec
;
;      Crouzet XT20 Millenium-II
;
; */


USE_PLL_CLOCK		.EQU		0
START			.EQU		FLASH_END

		#include	"board.asm"

		.macro idents_
                .byte	  "MR32",0    	; soft SCI
        .endm

; J2_P8  / PB1  ist  TxD
; J2_P10 / PB0  ist  RxD

;_TXDPORT         .EQU     PTA           ; <<<
;_TXDPIN          .EQU     0             ; <<<
;_SCITXINV        .EQU     0             ; (1 if SCI TX is inverted (no drivers))
;_SCIRXINV        .EQU     SCITXINV      ; (1 if SCI RX is inverted (no drivers))
;_RXDISIRQ        .EQU     0             ; (if RXD uses IRQ pin instead)

;              .IF RXDISIRQ = 0         ; RXDPORT & RXDPIN is defined
;_RXDPORT         .EQU     PTA           ; <<<
;_RXDPIN          .EQU     0             ; <<<
;_RXDPUEN         .EQU     1             ; use pull-up feature
;			.ELSE
;_RXDPUEN         .EQU     0             ; undefine pull-up enabling port
;              #endif

BUSCLOCK        .EQU     F_CPU        ; <<< MODIFY SO IT IS NEAR YOUR BUS CLOCK!
SCISPEED        .EQU     115200

;CALENABLED      .EQU     1             ; calibration enabled

;              .IF CALENABLED = 0
; SPEED           .EQU     5			;4.915             ; specify Xtal clk in MHz if no calibration (known freq.)
;              #endif

CONFIG1			.EQU		CONFIG

	#ifndef RAM_START
RAM_START        	.EQU    	$60
	#endif
    
	#ifndef RAM_SIZE
RAM_SIZE		.EQU		768
	#endif

FLASH_START		.EQU	    $8000       ; flash Start for Jx3

ROM_START     		.EQU    	$FE10	; Monitor

LOWEST_VECTOR_ADDR   	.EQU		$FFD2

;  TODO: calculate bootloader size, subtract from highest flash address
;	#ifndef FLASH_END
;FLASH_END		.EQU	 	$FA00       ; this is APL_VECT address (also from PRM file)
;	#endif

FLBPRMASK       	.EQU     	$8000       ; this is CPU specific FLBPR mask (i.e. bits that are always in the address)

ERBLK_LEN	  		.EQU	 128
WRBLK_LEN	    	.EQU	 64


MONITOR_START		.EQU		$FE00

;
;		.MACRO	dbnz
;				.DC.b		$3B,{1},(. - {2})
;		.ENDM
;
