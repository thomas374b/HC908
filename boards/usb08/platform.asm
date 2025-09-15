;/*
; * platform.asm  of  usb08-board  for hc08sprg bootloader
; *
; *  Created on: 15.06.2020
; *      Author: pantec
; */

			.PROCESSOR		68908


		#include        "mc68hc908jb8.asm"
		
;    #ifndef MC68HC908_MACROS_DEFINED
		#include	"macros.asm"
;    #endif


	.macro idents_
                .byte     "JB8",0    	; soft SCI
        .endm

;
; so called "user"-serial port 
;	26-pin header X1
;		X1_23 / PTC0  is  TxD
;		X1_18 / PTA7  is  RxD
;

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

	#ifndef APPL_RAM_START
		#if (USE_BOOTLOADER > 0)
APPL_RAM_START        	.EQU    	$48		
		#else	
APPL_RAM_START        	.EQU    	$40
		#endif
	#endif
    
	#ifndef APPL_RAM_SIZE
		#if (USE_BOOTLOADER > 0)
APPL_RAM_SIZE		.EQU		248
		#else		
APPL_RAM_SIZE		.EQU		256
		#endif
	#endif


	;  TODO: calculate bootloader size, subtract from highest flash address
	#ifndef APPL_FLASH_END
		#if (USE_BOOTLOADER > 0)
APPL_FLASH_END		.EQU	 	 $FA00       ; this should be APL_VECT address (also from PRM file)
		#else
APPL_FLASH_END		.EQU	 	$10000       
		#endif
	#endif


FLASH_START			.EQU	    $DC00       ; flash Start for Jx3
ROM_START     	.EQU    	$FC00		; Monitor


FLBPRMASK       .EQU     	$E000       ; this is CPU specific FLBPR mask (i.e. bits that are always in the address)

ERBLK_LEN	  		.EQU	 512
WRBLK_LEN	    	.EQU	 64

;
;		.macro	dbnz
;				.byte		$3B,{1},(. - {2})
;		.endm
;

	
			#include	"reg68hc908jb8.asm"
			
			
			
			
