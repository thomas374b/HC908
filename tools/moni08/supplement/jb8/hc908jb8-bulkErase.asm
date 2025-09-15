;
;
;   small program to load into RAM and delete the flash eeprom
;
;
	#include	"board.asm"
	#include	"fancy_macros.asm"


_ldProgStart	.EQU	  RAM_START + $06

	.ORG	RAM_START
	.DS		6,$DC

	.ORG	_ldProgStart
	.word	FLASH_START

	.ORG	fwCtrlByte
	.byte	[1 << 6]		; mass erase
;	.byte	0

	.ORG	fwCpuSpeed
	.byte	24

	.ORG	fwLastAddr
	.word	(FLASH_START+WRBLK_LEN)

	.ORG	fwDataBlock
	.DS		WRBLK_LEN,$FF
	

MainStart:
	; move stackpointer to the end of physical RAM
	ldHX	#StackPointer
	tHXS

	; enable LED output	
	store_reg	DDRD,#(LED0|LED1|LED2)	
	; lit one LED
	store_reg	PTD,#LED0			

	store_reg	FLBPR, #0xFF
	
	mov	fwCtrlByte, #[1 << 6] 		; flag for mass erase
;	mov	fwCpuSpeed, #24 					; 4*F_CPU MHz  ; patched from downloader
		
	ldHX	#_vect_TIM0
	
	jsr 	FlashErase

  
;  from datasheet
; 1. Set both the ERASE bit and the MASS bit in the FLASH control register.
; 2. Write any data to any FLASH address within the address range $FFE0-$FFFF.
; 3. Wait for a time, tnvs (5 #s).
; 4. Set the HVEN bit.
; 5. Wait for a time t me (2 ms).
; 6. Clear the ERASE bit.
; 7. Wait for a time, tnvh1 (100 #s).
; 8. Clear the HVEN bit.
; 9. After time, t rcv (1 #s), the memory can be accessed in read mode	again

	ldA		#"R"		; send ready signal
	jsr		WriteSerial
	    
  bsr Delay250ms  

	store_reg	PTD,#LED2	; lit the other LED

	swi				; return to monitor mode
	bra		MainStart


Delay250ms:
	pshA
	ldA	#250
d250loop:	
	bsr	Delay1ms
	dbnzA	d250loop			
	pulA
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

WordDelay:			
	dbnzX	WordDelay			;	((3*X)+3)*A) +13	= 2605
	dbnzA	WordDelay			;
	rts


	


	.ORG		$FE
	.word		MainStart

	.ORG		[RAM_START+RAM_SIZE-2]
StackPointer:


