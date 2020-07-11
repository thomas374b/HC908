
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

		.trace

		#include	"fancy_macros.asm"

RCS_ENA         			.EQU	1     	; READ COMMAND SUPPORTED?
WITH_RESIDENT_BUSY_LOOP		.EQU	0		; not enough space available
 
  #if (RCS_ENA != 0)
RCS             .EQU     $80   		; READ COMMAND SUPPORTED
  #else
RCS             .EQU     0     		; READ COMMAND unSUPPORTED
  #endif
 
VER_NUM         .EQU     1     		; FC protocol version number


;**************************************************************************

; no firmware rom flash routines
;_GETBYTE         .EQU     ROM_START+0
;_RDVRRNG         .EQU     ROM_START+3		; FlashRead
;_ERARNGE         .EQU     ROM_START+6		; FlashErase
;_PRGRNGE         .EQU     ROM_START+9		; FlashProg
;_DELNUS          .EQU     ROM_START+12		; DelayUS
; CTRLBYT         .EQU     RAM_START+$08
; CPUSPD          .EQU     RAM_START+$09
; _Laddr           .EQU     RAM_START+$0A
; _dataBuf        .EQU     RAM_START+$0C
 
; Jx1_FLS_BEG		  .EQU	    $F600       ; flash Start for Jx1

;**************************************************************************

		#if (PLATFORM == 9)
			#include	"platform.asm"
		#else
			#error "incomplete platform definition"
		#endif
 
;************************************************************************** 


INT_VECT	    .EQU	 LOWEST_VECTOR_ADDR

;
;************************************************************************** 

 
 
WR_DATA     .EQU     "W"
RD_DATA     .EQU     "R"
ENDPRG      .EQU     "Q"
ERASE__     .EQU     "E"
ACK         .EQU     $FC
IDENT       .EQU     "I"
 
; review timings
T1US	  	.EQU		1
T5US	  	.EQU		3
T10US	  	.EQU		5
T30US	  	.EQU		13

T1MS	  	.EQU		3
T2MS	  	.EQU		4
T4MS	  	.EQU		7
T10MS	  	.EQU		17

T100MS_      .EQU     	255

 ;*******************************************************************************************
		.macro	d_ms
  			ldA	{1}		  		; [2] ||
_L2_{2}:	clrX		  		; [1] ||
_L1_{2}:
			dbnzX	_L1_{2}		; [3] |    256*3 = 768T
  			dbnzA	_L2_{2}		; [3] || (768+3)*(arg-1) + 2 T
  		.endm

		.macro d_us
  			ldA	{1}				; [2]
_L1_{2}:
  			dbnzA	_L1_{2}		; [3] 3*(arg-1) + 2 T
  		.endm

;*******************************************************************************************


    .macro ilop
         	.byte    $32             ; this is illegal operation code
    .endm
 
 
;*******************************************************************************************

ERASE_ALG_SIZE	.EQU	(ERASE_ALG_END - ERASE_ALG)
PRG_ALG_SIZE	.EQU	(WR_ALG_END - WR_ALG)
RAM_END			.EQU	(RAM_START+RAM_SIZE)

DYN_RAM_CODE_SIZE	.EQU	0x50		; max of { ERASE_ALG_SIZE, PRG_ALG_SIZE } +16 byte safety

MY_ZEROPAGE		.EQU		RAM_START

next_var		.set		RAM_START

		declare_var		ADRS,2
		declare_var		POM,1
		declare_var		LEN,1
		declare_var		STAT,1
		declare_var		STSRSR,1
		declare_var		STACK,2
		declare_var		SOURCE,2

		declare_var		PRG,DYN_RAM_CODE_SIZE
		declare_var		DAT,WRBLK_LEN

;ADRS:	DS.W	1
;POM:	DS.B	1
;LEN:	DS.B	1
;STAT:	DS.B	1
;STSRSR: DS.B    1               ; storage for SRSR reg.
;STACK   DS.W    1               ; storage for stack
;SOURCE  DS.W    1               ; storage for source address
;DEFAULT_RAM:    SECTION
;PRG:	DS.B	100
;DAT:	DS.B	WRBLK_LEN


TESTING		.EQU 	0			; must be zero for production version

		#if (TESTING > 0)
; verrückter
			.ORG 		0
			.DS			PRG,0xdd
			.DS			DYN_RAM_CODE_SIZE,$cc
			.DS			WRBLK_LEN,$bb
			.DS			(RAM_END - .),0x

			.ORG		RAM_END
RamEnd:
		#endif
;*******************************************************************************************

		.ORG		FLASH_END			; should be section modulo erase page minus 32
APL_VECT:

PRI:    .byte    0,$80	; ,0,0,0,0,0,0 ; 8 bytes reserved for bootloader's private use
;*******************************************************************************************
SCIAPIREF:
 		.word   SCIINIT         ; this address holds the Start of SCI API table
 		.word	WRITE
	#if WITH_RESIDENT_BUSY_LOOP > 0
 		.word	MoniRomDelayLoop
	#else
		.word	0
	#endif
VEC0:
	jmp     main            ; vector 0		; RESET ist not an interrupt
VEC1:
	jmp     main            ; vector 1
VEC2:   jmp     main            ; vector 2
VEC3:   jmp     main            ; vector 3
VEC4:   jmp     main            ; vector 4
VEC5:   jmp     main            ; vector 5
VEC6:   jmp     main            ; vector 6
VEC7:   jmp     main            ; vector 7
VEC8:   jmp     main            ; vector 8
VEC9:   jmp     main            ; vector 9
VEC10:  jmp     main            ; vector 10
VEC11:  jmp     main            ; vector 7
VEC12:  jmp     main            ; vector 7
VEC13:  jmp     main            ; vector 7
VEC14:  jmp     main            ; vector 7
VEC15:  jmp     main            ; vector 7
VEC16:  jmp     main            ; vector 7
VEC17:  jmp     main            ; vector 7
VEC18:  jmp     main            ; vector 7
VEC19:  jmp     main            ; vector 7
VEC20:  jmp     main            ; vector 7
VEC21:  jmp     main            ; vector 7
VEC22:  jmp     main            ; vector 7

LastUserVec:

;*******************************************************************************************

		alignspace	PrivGap,32
SCIAPI:
        .word   SCIINIT         ; address of WRITE call
        .word   READ            ; address of READ call
        .word   WRITE           ; address of WRITE call
		.word	WRITE_LOOP      ; Start address in HX, length in LEN
		.word   LEN
	#if WITH_RESIDENT_BUSY_LOOP > 0
        .word   MoniRomDelayLoop
	#endif

		alignspace	VecGap,ERBLK_LEN

;		.DS		(ERBLK_LEN - (LastUserVec - APL_VECT)),$EE
; 		.ORG	FLASH_END					; must align to page erase size
;		#if (. - $FA00)
;			.ERROR "not aligned"
;		#endif

;		.DS		(main - .),$FD

;		.ORG	$FB00		; page aligned for protection

;*******************************************************************************************
main:     
        ldA     SRSR                    ; fetch RESET STATUS reg.
        stA     STSRSR                  ; store for later re-use
        tstA                            ; check is zero (this happens if RESET pulse is too short)
        beq     slfprg                  ; if so, jump to self programming
        and     #%10000000              ; mask only POR RESET source

	    bne	    slfprg                  ; any of these sources, go to self programming
        jmp     VEC0                    ; any of other sources, jump to real application

;		beq		VEC0					; alternative for the two above but branch out of range here

slfprg:  
		ldA		#0xFF
		stA		COPCTL
        mov     #%10000001,CONFIG1      ; COP disable

        ldHX	#[RAM_END-1]
        tHXS

;        ldA     #[[FLASH_END+ERBLK_LEN]/256]
;        ldA     #[[FLASH_END+32]/256]
;        stA     FLBPR                   ; is in Flash !!
            
        bsr     SCIINIT                 ; call SCI init now :)

     	ldA		SCS1
    	mov	  	#ACK,SCDR

        ldX     #T100MS_
L2:
		clrA
L1:
		brset	5,SCS1,CAUGHT
        dbnzA   L1
        dbnzX   L2
 
;       timeout 
ENDPRG_COM:
        ilop          ; generate RESET by doing illegal operation

;******************************************************************************************* 
SCIINIT:
        ; 4.1952MHz Xtal settings
        bset    6,SCC1                  ; SCI enable
        mov     #%00001100,SCC2         ; transmit & receive enable
      	clr	    SCC3
;      	mov	    #%00000001,SCBR    		; 9600Bd @ 4.915 MHz Xtal (SCI is driven by BUS clk, DataSheet rev 3.1)
		clr		SCBR				    ; 19200 Baud @ 4.915MHz Xtal
 
;        ldX     PRI+1                   ; [3B]
;        ldA     PRI                     ; [3B]
;        pshA                            ; [1B]
;        pulH                            ; [1B]
;        stHX    ONEBIT                  ; (2B)

        rts
 
;*******************************************************************************************
CAUGHT:			; CAUGHT IN SELF-PROGRAMMING?
    	bsr     READ
    	cmp  	#ACK
    	bne	  	ENDPRG_COM

;        mov     #SPEED,CPUSPD   ; feed the speed
;		clr     CTRLBYT         ; no mass erase

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
      #if (RCS_ENA == 1)
        cbeqA   RD_DATA, RD_DATA_COM   ; Read command
      #endif
 
        ; if no valid command found (including Quit) 
        ; generate reset too! 
        ilop          ; generate RESET by doing illegal operation
;******************************************************************************************* 
IDENT_COM:                      ; TRANSFER OF IDENTIFICATION STRING 
        mov     #ID_STRING1_END-ID_STRING1, LEN
        ldHX    #ID_STRING1
        bsr     WRITE_LOOP
 
;        ldHX    #ONEBIT         ; next 8 are from RAM area (!)
;        mov     #8,LEN          ; 8 is future priv area
;        bsr     WRITE_LOOP

        ldA     STSRSR          ; first byte will be SRSR reg.
        bsr     WRITE

        ldHX    #PRI+1     		; next 7 are from private area
        mov     #7,LEN
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
      #if (RCS_ENA == 1)
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
      #endif
;******************************************************************************************* 
READ:
        brclr	5,SCS1,READ
    	ldA		SCDR
    	rts

WRITE:
        brclr	6,SCS1,WRITE
    	stA	  	SCDR
    	rts

;*******************************************************************************************
GETADDR: 
        bsr     READ
        pshA
        pulH                    ; store to H:x
        bsr     READ
        tAX                     ; store to h:X
        rts
;******************************************************************************************* 

;*******************************************************************************************
ERASE_COM: 
 
    		bsr     READ
    		stA		ADRS
    		bsr     READ
    		stA		ADRS+1

    		ldHX	#ERASE_ALG		; LOAD ERASE ALGORITHM TO RAM
    		stHX    SOURCE
;    		mov		#ERASE_ALG_END-ERASE_ALG,STAT
 			mov		#ERASE_ALG_SIZE, STAT
 
        	bsr     CPY_PRG

    		jmp		PRG				; ERASE BLOCK
;******************************************************************************************* 
WR_DATA_COM: 
    		bsr     READ
    		stA		ADRS
    		bsr     READ
    		stA		ADRS+1
    		bsr     READ
    		stA		LEN
    		stA		STAT
    		ldHX	#DAT
WR_DATA_L1: 
    		bsr     READ
    		stA		,X
    		aiX		#1
    		dbnz	STAT,WR_DATA_L1
								; START OF SELF-PROGRAMMING
WR_BUF:
    		ldHX	#WR_ALG			; LOAD WRITE ALGORITHM TO RAM
    		stHX    SOURCE
;    		mov		#WR_ALG_END-WR_ALG, STAT
    		mov		#PRG_ALG_SIZE, STAT

        	bsr     CPY_PRG

        	tSHX
        	stHX    STACK           ; copy stack for later re-call

    		jmp		PRG

RETWR:  	ldHX    STACK
        	tHXS                     ; restore stack

        	jmp     SUCC
;*******************************************************************************************
CPY_PRG:
        	tSHX
        	stHX    STACK           ; copy stack for later re-call

    		ldHX	SOURCE			; LOAD WRITE ALGORITHM TO RAM
    		tHXS
    		ldHX	#PRG
CPY_PRG_L1:
    		pulA
    		stA		,X
    		aiX		#1
    		dbnz	STAT,CPY_PRG_L1
         
        	ldHX    STACK
        	tHXS                     ; restore stack
        	rts
;*******************************************************************************************
ERASE_ALG:

        	ldA     #%00000010
        	stA     FLCR            ; ERASE bit on
        	ldA     FLBPR           ; dummy read FLBPR

    		ldHX	ADRS            ; write anything
    		stA		,X              ; to desired range

        	d_us	#T10US, e10us	; wait 10us

        	ldA     #%00001010
	    	stA     FLCR    		; set HVEN, keep ERASE

    		d_ms	#T4MS, e4ms		; wait 4ms

        	ldA     #%00001000
    		stA		FLCR	        ; keep HVEN, ERASE off
    		d_us	#T5US, e5us		; wait 5us

        	clrA
    		stA		FLCR    		; HVEN off
    		d_us	#T1US, e1us		; wait 1us

    		jmp		SUCC            ; finish with ACK
ERASE_ALG_END:
;*******************************************************************************************
WR_ALG:
        	ldA     #%00000001
        	stA     FLCR            ; PGM bit on
        	ldA     FLBPR           ; dummy read FLBPR

        	ldHX	ADRS            ; prepare addrs'
        	stA     ,X              ; and write to desired range
        	d_us	#T10US, w10us	; wait 10us

        	ldA     #%00001001
    		stA		FLCR	        ; set HVEN, keep PGM
    		d_us	#T5US, w5us1	; wait 5us

        	ldHX	#DAT            ; prepare addrs'
    		tHXS
    		ldHX	ADRS
    		mov		LEN,POM
WR_ALG_L1:
    		pulA
    		stA		,X
    		aiX		#1
    		d_us	#T30US, w30us	; wait 30us
    		dbnz	POM,WR_ALG_L1   ; copy desired block of data

        	ldA     #%00001000
    		stA		FLCR	        ; keep HVEN, PGM off
    		d_us	#T5US, w5us2	; wait 5us

        	clrA
    		stA		FLCR    		; HVEN off
    		d_us	#T1US, w1us		; wait 1us

    		jmp		RETWR           ; finish with ACK (& restore STACK before)
WR_ALG_END:
;*******************************************************************************************

;SwitchToPLL:
;		switch_to_pll_clock
;		rts;

; cycles = (((A-3)*3 +10)*X) + 7
; longest wait time 25.1 s
		#if (WITH_RESIDENT_BUSY_LOOP > 0)

MoniRomDelayLoop:				   ; [5] ; jsr
		decA                       ; [1] J     $fd21   4a
_Lfd22:
		pshA                       ; .     $fd22   87
		decA                       ; [1] J     $fd23   4a
		decA                       ; [1] J     $fd24   4a
_Lfd25:
		dbnzA    _Lfd25            ; [3] K.    $fd25   4b fe
		pulA                       ; [2] .     $fd27   86
		dbnzX    _Lfd22            ; [3] ..    $fd28   5b f8
		rts                        ; [4] .     $fd2a   81
		#else


		#endif

;******************************************************************************************* 

ID_STRING2:
        idents_
ID_STRING2_END:


;*******************************************************************************************
;FL1_PROT_ROM:	;SECTION
;
; 14 bytes
ID_STRING1:
        .byte    VER_NUM | RCS   ; version number & "Read command supported" flag
        .word    FLASH_START     ; Start ADDRESS OF FLASH
        .word    FLASH_END       ; END ADDRESS OF FLASH
        .word    APL_VECT        ; POINTER TO APPLICATION VECTOR TABLE
        .word    INT_VECT        ; POINTER TO BEGINING OF FLASH INT. VECTORS
        .word    ERBLK_LEN       ; ERASE BLCK LENGTH OF FLASH ALG.
        .word    WRBLK_LEN       ; WRITE BLCK LENGTH OF FLASH ALG.
ID_STRING1_END:
		.byte	 0


		.DS		(MONITOR_START - .),$EE
		.ORG	MONITOR_START
   
;*******************************************************************************************

      	.ORG		LOWEST_VECTOR_ADDR	; address of last flash line

	declare_vector _vect_SCItransmit	,VEC22
	declare_vector _vect_SCIreceive		,VEC21
	declare_vector _vect_SCIerror		,VEC20
	declare_vector _vect_SPItransmit	,VEC19
	declare_vector _vect_SPIreceive		,VEC18
	declare_vector _vect_ADC			,VEC17
	declare_vector _vect_TIMOVF			,VEC16
	declare_vector _vect_TIM1			,VEC15
	declare_vector _vect_TIM0			,VEC14
	declare_vector _vect_TimAOvf		,VEC13
	declare_vector _vect_TimACh3		,VEC12
	declare_vector _vect_TimACh2		,VEC11
	declare_vector _vect_TimACh1		,VEC10
	declare_vector _vect_TimACh0		,VEC9
	declare_vector _vect_PWMmc			,VEC8
	declare_vector _vect_Fault4			,VEC7
	declare_vector _vect_Fault3			,VEC6
	declare_vector _vect_Fault2			,VEC5
	declare_vector _vect_Fault1			,VEC4
	declare_vector _vect_PLL			,VEC3
	declare_vector _vect_IRQ			,VEC2
	declare_vector _vect_SWI			,VEC1
	declare_vector _vect_RESET			,main


	.end






