;
;
;   small program to load into RAM and delete the flash eeprom
;
;
	#include	"board.asm"
	#include	"fancy_macros.asm"

	.ORG		RAM_START
	DC.b		0

	declare_byte	fwCtrlByte, [1 << 6]
	declare_byte	fwCpuSpeed, 12
;	declare_byte	fwLastAddr, [$DC00 + 8192 -1]
;	declare_byte	fwDataBlock, $db

	.ORG	fwCtrlByte    

	.ORG		[fwDataBlock + 64]

	alignspace	Dummy, 8

	declare_delay	100us,1,98

MainStart:
    .IF	0

        ldHX            #FLBPR                  ; need this address for mass erase
        jsr             BulkErase

        ldHX            #_vect_KBIR
        jsr             BulkErase

        ldHX            #FLASH_START
        jsr             BulkErase

	ldA		#"R"			; send ready signal
	jsr		WriteSerial
	
	swi					; return to monitor mode
	bra		MainStart

BulkErase:
	mov		#[1 << 6], fwCtrlByte
	mov		#12, fwCpuSpeed
	jsr		FlashErase
	rts

;MyBulkErase:
    #else
	store_reg	FLBPR, #0xFF
	store_reg	FLCR, #[MASS|ERASE]
	store_reg	$FFF0, #$0A		; need this address for mass erase
	ldA		#4
_shortDelay:
	dbnzA		_shortDelay		;	[3] *5  15
	set_bits	FLCR, HVEN
	delay2ms
	clear_bits	FLCR, ERASE
	bsr		Delay100us
	clear_bits	FLCR,HVEN

	bsr		Delay100us

	ldA		#"R"		; send ready signal
	jsr		WriteSerial

	swi				; return to monitor mode
	bra		MainStart

    #endif

;	rts


; pieps wenn fertig
;	store_reg	DDRD,#PTD3
;ReadyLoop:						; [247]
; 	store_reg	PTD,#PTD3
; 	bsr			Delay100us
; 	store_reg	PTD,#0			; [2+2]
; 	bsr			Delay100us				; [17+]
;	bra 		ReadyLoop		; [3]
;

	.ORG		$FE
StackPointer:
	.word		MainStart


