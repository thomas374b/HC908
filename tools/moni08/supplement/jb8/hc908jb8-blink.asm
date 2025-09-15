;
;  a simplified program to run from RAM in monotor-mode
;

    #include	"board.asm"
    #include	"fancy_macros.asm"

    .ORG   0x48		; first available RAM address for the JB8
			; the first 8 bytes might be used by the monitor

WordDelay:                      ; max 26.4ms @ 7.3 MHz
        dbnzA   WordDelay                       ;       ((3*A)+3)*X) +13
        dbnzX   WordDelay                       ;
        rts

BigDelay:
	ldA	250
	ldX	250
	jsr 	WordDelay
	rts

MainStart:
	ldHX	#StackPointer	; get some room by putting it at the end of available RAM
	tHXS

	store_reg	DDRD,#7		;data direction register to "OUT" for the lowermost 3 bits
	ldA	#0	;LEDs are pulled up to VDD need to tear the bit down to switch them on
	stA	PTD	; all on

	ldA	#240
blinkLoop:
	stA	PTD
	pshA
	pshX
	jsr	WriteSerial
	jsr	BigDelay
	pulX
	pulA
	dbnzA	blinkLoop

	ldA	#"X"
	jsr	WriteSerial
    
	ldA	#7		; LEDs off
	stA	PTD

        swi                             ; return to monitor mode
        bra             MainStart	; safety paranoia


    .ORG	$FE		; backward compatibility stack-pointer position for RAM start
    .word	MainStart


    .ORG	[RAM_START+RAM_SIZE-2]
StackPointer:


