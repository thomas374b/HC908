;/*
; * platform.asm  of  usb08-board  for hc08sprg bootloader
; *
; *  Created on: 15.06.2020
; *      Author: pantec
; */

			.PROCESSOR		68908

		.INCLUDE	macros.asm

		.INCLUDE	reg68hc908jb8.asm

		.MACRO idents_
                .DC.b     "JB8",0    	; soft SCI
        .ENDM

; J2_P8  / PB1  ist  TxD
; J2_P10 / PB0  ist  RxD

TXDPORT         .EQU     PTA           ; <<<
TXDPIN          .EQU     0             ; <<<
SCITXINV        .EQU     0             ; (1 if SCI TX is inverted (no drivers))
SCIRXINV        .EQU     SCITXINV      ; (1 if SCI RX is inverted (no drivers))
RXDISIRQ        .EQU     0             ; (if RXD uses IRQ pin instead)

              .IF RXDISIRQ = 0         ; RXDPORT & RXDPIN is defined
RXDPORT         .EQU     PTA           ; <<<
RXDPIN          .EQU     0             ; <<<
RXDPUEN         .EQU     1             ; use pull-up feature
			.ELSE
RXDPUEN         .EQU     0             ; undefine pull-up enabling port
              .ENDIF

BUSCLOCK        .EQU     3000000       ; <<< MODIFY SO IT IS NEAR YOUR BUS CLOCK!
SCISPEED        .EQU     9600

CALENABLED      .EQU     1             ; calibration enabled

              .IF CALENABLED = 0
SPEED           .EQU     6             ; specify Xtal clk in MHz if no calibration (known freq.)
              .ENDIF

CONFIG1			.EQU		CONFIG

	.IFNCONST RAM_START
RAM_START        	.EQU    	$40
	.ENDIF
    
	.IFNCONST RAM_SIZE
RAM_SIZE		.EQU		256
	.ENDIF

FLASH_START		.EQU	    $DC00       ; flash Start for Jx3

ROM_START     		.EQU    	$FC00		; Monitor

LOWEST_VECTOR_ADDR   	.EQU		$FFF0

;  TODO: calculate bootloader size, subtract from highest flash address
;	.IFNCONST FLASH_END
;FLASH_END		.EQU	 	$FA00       ; this is APL_VECT address (also from PRM file)
;	.ENDIF

FLBPRMASK       	.EQU     	$E000       ; this is CPU specific FLBPR mask (i.e. bits that are always in the address)

ERBLK_LEN	  		.EQU	 512
WRBLK_LEN	    	.EQU	 64

;
;		.MACRO	dbnz
;				.DC.b		$3B,{1},(. - {2})
;		.ENDM
;
