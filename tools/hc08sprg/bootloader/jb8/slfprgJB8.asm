
;********************************************************************* 
; HEADER_Start
; 
;  	   $File Name: slfprgR-jk3.asm$ 
;      Project:        Developper's HC08 Bootloader Slave 
;      Description:    JK/JL main bootloader file 
;      Platform:       HC08 
;      $Version: 6.0.22.0$ 
;      $Date: Feb-22-2006$  
;      $Last Modified By: r30323$ 
;      Company:        Freescale Semiconductor 
;      Security:       General Business 
; 
; ===================================================================  
; Copyright (c):      Freescale Semiconductor, 2004, All rights reserved. 
; 
; ===================================================================  
; THIS SOFTWARE IS PROVIDED BY FREESCALE "AS IS" AND ANY 
; EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
; IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
; PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL FREESCALE OR 
; ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
; SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
; NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
; HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
; STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
; ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED 
; OF THE POSSIBILITY OF SUCH DAMAGE. 
; =================================================================== 
; 
; HEADER_END 

 
RCS_ENA         .EQU     1     		; READ COMMAND SUPPORTED?
 
  .IF RCS_ENA != 0
RCS             .EQU     $80   		; READ COMMAND SUPPORTED
  .ELSE
RCS             .EQU     0     		; READ COMMAND unSUPPORTED
  .ENDIF
 
VER_NUM         .EQU     1     		; FC protocol version number


INT_VECT	    .EQU	 LOWEST_VECTOR_ADDR


GETBYTE         .EQU     ROM_START+0
RDVRRNG         .EQU     ROM_START+3
ERARNGE         .EQU     ROM_START+6
PRGRNGE         .EQU     ROM_START+9
DELNUS          .EQU     ROM_START+12


CTRLBYT         .EQU     RAM_START+$08
CPUSPD          .EQU     RAM_START+$09
LADDR           .EQU     RAM_START+$0A
DATA            .EQU     RAM_START+$0C
 
; Jx1_FLS_BEG		  .EQU	    $F600       ; flash Start for Jx1


;************************************************************************** 
;            .IF PLATFORM = 1
; 				.INCLUDE	platform1.asm
;            .ENDIF
;            .IF PLATFORM = 2
; 				.INCLUDE	platform2.asm
;            .ENDIF
;            .IF PLATFORM = 3
; 				.INCLUDE	platform3.asm
;            .ENDIF
;            .IF PLATFORM = 4
;				.INCLUDE	platform4.asm
;            .ENDIF
;            .IF PLATFORM = 5
;				.INCLUDE	platform5.asm
;            .ENDIF
;            .IF PLATFORM = 6
;            	.INCLUDE	platform6.asm
;            .ENDIF
;            .IF PLATFORM = 7
;				.INCLUDE	platform7.asm
;            .ENDIF
;            .IF PLATFORM = 8
;				.INCLUDE	platform8.asm
;            .ENDIF
;**************************************************************************

            .IF PLATFORM = 9
				.INCLUDE	platform.asm
			.ELSE
				.ERROR "incomplete platform definition"
            .ENDIF
 
;************************************************************************** 

;************************************************************************** 
 
          .IF RXDISIRQ = 1           ; RXDPORT & RXDPIN is *not* defined
            .IF SCIRXINV = 1
CONFIG2DEF      .EQU     %10000000     ; pullup on IRQ disabled!   you need hardwired pull-down in fact!
            .ELSE
CONFIG2DEF      .EQU     %00000000     ; pullup on IRQ enabled!
            .ENDIF
          .ELSE
CONFIG2DEF      .EQU     %00000000     ; pullup on IRQ *not* disabled!
RXDDDR          .EQU     RXDPORT+4
            .IF RXDPUEN = 1
RXDPUE          .EQU     RXDPORT+$0B   ; define pull-up enable port
            .ENDIF
          .ENDIF
 
TXDDDR          .EQU     TXDPORT+4
    
SCITXTICK       .EQU     (BUSCLOCK/SCISPEED)
 
;************************************************************************** 
   .IF RXDISIRQ = 0         ; RXDPORT & RXDPIN is defined
    .IF (RXDPORT = TXDPORT) & (RXDPIN = TXDPIN)
SINGLEWIRE      .EQU     1             ; do use single-wire feature
    .ELSE
SINGLEWIRE      .EQU     0             ; do NOT use single-wire feature
    .ENDIF
   .ELSE
SINGLEWIRE      .EQU     0             ; do NOT use single-wire feature
   .ENDIF
;******************************************************************************************* 
 
 
WR_DATA     .EQU     "W"
RD_DATA     .EQU     "R"
ENDPRG      .EQU     "Q"
ERASE__     .EQU     "E"
ACK         .EQU     $FC
IDENT       .EQU     "I"
 
T100MS      .EQU     255
 
       .MACRO ilop
            	.DC.b    $32             ; this is illegal operation code
       .ENDM
 
   .MACRO skip1
            .DC.b    $21             ; braNCH NEVER (saves memory)
           .ENDM
 
       .MACRO	skip2
            .DC.b    $65             ; CPHX (saves memory)
            .ENDM
 
     .MACRO brrxdlo
     	.IF RXDISIRQ = 1
      		.IF SCIRXINV = 1
    			bih     {1}       ; branch if RXD low
      		.ELSE
    			bil     {1}       ; branch if RXD low
      		.ENDIF
    	.ELSE    ; RXD uses normal I/O pin
      		.IF SCIRXINV = 1
    			brset   RXDPIN,RXDPORT,{1}    ; branch if RXD low
      		.ELSE
    			brclr   RXDPIN,RXDPORT,{1}   ; branch if RXD low
      		.ENDIF
    	.ENDIF
     .ENDM
 
    .MACRO	brrxdhi
 
    .IF RXDISIRQ = 1
      .IF SCIRXINV = 1
    bil     {1}       ; branch if RXD hi
      .ELSE
    bih     {1}       ; branch if RXD hi
      .ENDIF
    .ELSE    ; RXD uses normal I/O pin
      .IF SCIRXINV = 1
    brclr   RXDPIN,RXDPORT,{1}    ; branch if RXD hi
      .ELSE
    brset   RXDPIN,RXDPORT,{1}    ; branch if RXD hi
      .ENDIF
    .ENDIF
 
    .ENDM
 
    .MACRO	txdclr
 
      .IF SCITXINV = 1
        bset    TXDPIN,TXDPORT  ; clr bit
      .ELSE
        bclr    TXDPIN,TXDPORT  ; clr bit
      .ENDIF
 
        .ENDM
 
    .MACRO	txdset
 
      .IF SCITXINV = 1
        bclr    TXDPIN,TXDPORT  ; set bit
      .ELSE
        bset    TXDPIN,TXDPORT  ; set bit
      .ENDIF



      .ENDM
         
TMOD    .EQU     TMODH           ; we use high address for storing all 16 bits at once
 
;*******************************************************************************************
;			.ORG	RAM_START
;MY_ZEROPAGE:    ; SECTION  SHORT
 
;ONEBIT: 	.DS.w    1
;BITS:   	.DS.b    1
;ADRS:   	.DS.w    1
;LEN:    	.DS.b    1
;STATUS:   	.DS.b    1
;STSRSR: 	.DS.b    1               ; storage for SRSR reg.

MY_ZEROPAGE		.EQU		RAM_START

next_var		.SET		RAM_START

	declare_var		ONEBIT,2
	declare_var		BITS,1
	declare_var		ADRS,2
	declare_var		LEN,1
	declare_var		STATUS,1
	declare_var		STSRSR,1

; verrückter
;		.ORG 		0
;		.DS			RAM_START,0xEA
;		.DS			(RAM_START+RAM_SIZE),0xa0
;*******************************************************************************************

			.ORG		FLASH_END			; should be section modulo erase page minus 32
APL_VECT:

PRI:							 ; address of onebit-ticks default value
		.DC.w    SCITXTICK
        .DC.w    0
        .DC.w    0
        .DC.w    0

VEC0:   jmp     main            ; vector 0		; RESET ist not an interrupt
VEC1:   jmp     main            ; vector 1
VEC2:   jmp     main            ; vector 2
VEC3:   jmp     main            ; vector 3
VEC4:   jmp     main            ; vector 4
VEC5:   jmp     main            ; vector 5
VEC6:   jmp     main            ; vector 6
VEC7:   jmp     main            ; vector 7

; 		.ORG	FLASH_END					; must align to page erase size
;		.IF (. - $FA00)
;			.ERROR "not aligned"
;		.ENDIF

;******************************************************************************************* 
main:     
        ldA     RSR                     ; fetch RESET STATUS reg.
        stA     STSRSR                  ; store for later re-use
        tstA                            ; check is zero (this happens if RESET pulse is too short)
        beq     slfprg                  ; if so, jump to self programming
        and     #%10000000              ; mask only POR RESET source
        beq     VEC0                    ; any of these sources, go to self programming
slfprg:  
;      .IF CONFIG2DEF = 0
;        clr     CONFIG2
;      .ELSE
;        mov     #CONFIG2DEF,CONFIG2     ; from definition
;      .ENDIF
        mov     #%10000001,CONFIG1      ; COP disable
 
;        ldA     #[[FLASH_END+ERBLK_LEN]/256]
        ldA     #[[FLASH_END+32]/256]
        stA     FLBPR                   ; is in RAM!!
            
        bsr     SCIINIT                 ; call SCI init now :)
         
    	ldHX  	#SCITXTICK				; ONEBIT is rewritten to default value
    	stHX	  ONEBIT 					; just for case the private area is erased
		 
        ldA     #ACK
        jsr     WRITE
         
        ldX     #T100MS
 
LL2:    clrA
LL1:
		brrxdhi L1
         
        dbnzA   LL1
        dbnzX   LL2
 
;       timeout 
        ilop          ; generate RESET by doing illegal operation
;******************************************************************************************* 
 
L2:
		clrA
L1:
		brrxdlo CAUGHT
         
        dbnzA   L1
        dbnzX   L2
 
;       timeout 
        ilop          ; generate RESET by doing illegal operation
;******************************************************************************************* 
 
SCIINIT: 
        ldX     PRI+1                   ; [3B]
        ldA     PRI                     ; [3B]
 
        pshA                            ; [1B]
        pulH                            ; [1B]
        stHX    ONEBIT                  ; (2B)
 
    .IF RXDISIRQ = 0         ; RXDPORT & RXDPIN is defined (not IRQ)
        bclr    RXDPIN,RXDDDR           ; input for RXD
      .IF RXDPUEN = 1
        bset    RXDPIN,RXDPUE
      .ENDIF
    .ENDIF
 
        txdset
    .IF SINGLEWIRE=0
        bset    TXDPIN,TXDDDR           ; (2B) output for TXD
    .ENDIF
        rts
 
;******************************************************************************************* 
 
CAUGHT:            ; CAUGHT IN SELF-PROGRAMMING? 
        jsr     SCIRXNOEDGE
 
      .IF CALENABLED = 1
        clrH
        clrX
 
MONRXD2: 
        brrxdhi         MONRXD2
 
CHKRXD: 
        brrxdhi         BRKDONE
 
      .IF RXDISIRQ = 1
        nop                     ; (1)
        nop                     ; (1)
      .ENDIF
        aiX     #1              ; (2) INCREMENT THE COUNTER
        bra     CHKRXD          ; (3) GO BACK AND CHECK SIGNAL AGAIN
 
BRKDONE: 
        stHX    ONEBIT          ; store it
 
        tXA
        ldX     #$1a            ; calculate speed
        div
      .ELSE
        ldA     #16             ; <<< modify this if no calibration is required (BUS freq in MHz * 4)
      .ENDIF
              
        stA     CPUSPD
        clr     CTRLBYT         ; no mass erase
;******************************************************************************************* 
; successful return from all write routines 
SUCC: 
        ldA     #ACK
        jsr     WRITE
 
;fall thru to background 
;******************************************************************************************* 
; BEGIN OF BACKGROUND COMMAND LOOP 
BCKGND: 
        bsr     READ
             
        cbeqA   ERASE__, ERASE_COM       ; Erase command
        cbeqA   WR_DATA, WR_DATA_COM   ; Write (program) command
        cbeqA   IDENT, IDENT_COM       ; Ident command
      .IF RCS_ENA = 1
        cbeqA   RD_DATA, RD_DATA_COM   ; Read command
      .ENDIF
 
        ; if no valid command found (including Quit) 
        ; generate reset too! 
        ilop          ; generate RESET by doing illegal operation
;******************************************************************************************* 
IDENT_COM:                      ; TRANSFER OF IDENTIFICATION STRING 
        mov     #ID_STRING1_END-ID_STRING1, LEN
        ldHX    #ID_STRING1
        bsr     WRITE_LOOP
 
        ldHX    #ONEBIT         ; next 8 are from RAM area (!)
        mov     #8,LEN          ; 8 is future priv area
        bsr     WRITE_LOOP
 
        mov     #ID_STRING2_END-ID_STRING2, LEN
        ldHX    #ID_STRING2
        bsr     WRITE_LOOP
 
        bra     BCKGND          ; finish without ACK
       
;******************************************************************************************* 
WRITE_LOOP:                     ; Start address in HX, length in LEN
        ldA     ,X
        jsr     WRITE
        aiX     #1
        dbnz    LEN, WRITE_LOOP
        rts
;******************************************************************************************* 
      .IF RCS_ENA = 1
RD_DATA_COM: 
 
        bsr     READ
        stA     ADRS
        bsr     READ
        stA     ADRS+1
        bsr     READ
        stA     LEN
        ldHX    ADRS
         
        bsr     WRITE_LOOP
 
        bra     BCKGND          ; finish without ACK
      .ENDIF
;******************************************************************************************* 
GETADDR: 
        bsr     READ
        pshA
        pulH                    ; store to H:x
        bsr     READ
        tAX                     ; store to h:X
        rts
;******************************************************************************************* 
ERASE_COM: 
        bsr     GETADDR         ; read adr. and return in H:X
        jsr     ERARNGE         ; call ROM erase routine
 
        bra     SUCC            ; refer STATUSus back to PC
 
;******************************************************************************************* 
WR_DATA_COM: 
        bsr     GETADDR         ; read adr. and return in H:X
        stHX    ADRS
        bsr     READ
        stA     STATUS
        decA
        stA     LEN
        ldHX    #DATA
WR_DATA_L1: 
        bsr     READ
        stA     ,X
        aiX     #1
        dbnz    STATUS,WR_DATA_L1
                                ; Start OF SELF-PROGRAMMING
        ldA     ADRS+1
        add     LEN
        stA     LADDR+1
        ldA     ADRS
        adc     #0
        stA     LADDR
        ldHX    ADRS
        jsr     PRGRNGE         ; call ROM erase routine
         
        jmp	    SUCC            ; refer STATUS back to PC
 
;******************************************************************************************* 
READ: 
        brrxdlo READ            ; (3B) loop until RXD high (idle)
 
SCIRXNOEDGE: 
        pshH                    ; (1B)
        pshX                    ; (1B)
 
        ldX     ONEBIT          ; (2B)
        ldA     ONEBIT+1        ; (2B)
        lsrX                    ; (1B)
        rorA                    ; (1B)
        stX     TMODH           ; first TOF should occur just half bit
        stA     TMODL           ; after the hi-lo edge (right in the middle of Start bit)
 
SCIRX1: 
        brrxdhi SCIRX1          ; loop until RXD low (wait for Start bit)
 
        mov     #%00010000,TSC  ; initialize prescalers, reset & run the timer
        mov     #9,BITS         ; number of bits + 1
         
SCIRX2: brclr   7,TSC,SCIRX2    ; wait for TOF
                                 
        lsrA                    ; shift data right (highest bit cleared)
        brrxdlo  RXDLOW          ; skip if RXD low
        orA     #$80            ; set highest bit if RXD high
 
RXDLOW: ldHX    ONEBIT
        stHX    TMOD
 
        bclr    7,TSC           ; and clear TOF
        dbnz    BITS,SCIRX2     ; and loop for next bit
 
        bra     EPILOG
 

;CODE_ROM:	; SECTION

page3:          
         
SCITX0: brclr   7,TSC,SCITX0    ; wait for TOF
        mov     #10,BITS        ; number of bits + 1 stop bit
        bra     DATALOW         ; jump to loop
 
SCITX2:         
        sec                     ; set carry so stop bit is '1'
        rorA                    ; rotate lowest bit
        bcc     DATALOW
 
        txdset
        skip2                   ; skip next two bytes
DATALOW: 
        txdclr
        bclr    7,TSC           ; and clear TOF
SCITX1: brclr   7,TSC,SCITX1    ; wait for TOF
 
        dbnz    BITS,SCITX2     ; and loop for next bit
 
EPILOG: 
        mov     #%00110000,TSC  ; stop & reset timer
 
    .IF SINGLEWIRE=1
        bclr    TXDPIN,TXDDDR           ; (2B) input for TXD (shared with RXD)
    .ENDIF
        pulX
        pulH
        rts

;*******************************************************************************************
FL2_PROT_ROM:	; SECTION
; 16 bytes
WRITE:
        pshH                    ; (1B)
        pshX                    ; (1B)

        ldHX    ONEBIT          ; (2B)
        stHX    TMOD            ; (2B)

    .IF SINGLEWIRE=1
        bset    TXDPIN,TXDDDR   ; (2B) output for TXD
    .ENDIF

        mov     #%00010000,TSC  ; (3B) initialize prescalers, reset & run the timer
        jmp     page3           ; (3B) total 14 (of 14)


;*******************************************************************************************

SCIAPI:
        .DC.w    SCIINIT         ; address of WRITE call
        .DC.w    READ            ; address of READ call
        .DC.w    WRITE           ; address of WRITE call
        .DC.w    ONEBIT          ; address of ONEBIT variable

ID_STRING2:
        idents_
ID_STRING2_END:



;*******************************************************************************************
FL1_PROT_ROM:	;SECTION
;
; 14 bytes
ID_STRING1:
        .DC.b    VER_NUM | RCS   ; version number & "Read command supported" flag
        .DC.w    FLASH_START     ; Start ADDRESS OF FLASH
        .DC.w    FLASH_END       ; END ADDRESS OF FLASH
        .DC.w    APL_VECT        ; POINTER TO APPLICATION VECTOR TABLE
        .DC.w    INT_VECT        ; POINTER TO BEGINING OF FLASH INT. VECTORS
        .DC.w    ERBLK_LEN       ; ERASE BLCK LENGTH OF FLASH ALG.
        .DC.w    WRBLK_LEN       ; WRITE BLCK LENGTH OF FLASH ALG.
ID_STRING1_END:
		.DC.b	 0


SCI_PROT_ROM:	;SECTION
; 2 bytes 
SCIAPIREF: 
        .DC.w    SCIAPI          ; this address holds the Start of SCI API table
                                ; this is an address of READ call, 
                                ; WRITE call is 2 bytes higher 
   

;*******************************************************************************************

      	.ORG		$FFF0	; address of last flash line

	declare_vector _vect_KBIR	,VEC7
	declare_vector _vect_TIMOVF	,VEC6
	declare_vector _vect_TIM1	,VEC5
	declare_vector _vect_TIM0	,VEC4
	declare_vector _vect_IRQ	,VEC3
	declare_vector _vect_USB	,VEC2
	declare_vector _vect_SWI	,VEC1
	declare_vector _vect_RESET	,main


.END






