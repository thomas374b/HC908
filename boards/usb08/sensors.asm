
WITH_TIMER_INT		.EQU	0
WITH_LOOP_SAFETY	.EQU 	0

		.INCLUDE	board.asm


RamBegin	.EQU $0041

R2Seq 		.EQU RamBegin		;_v@0041
R0Seq		.EQU R2Seq+1		;_v@0042
SetupPtr	.EQU R0Seq+1		;_v@0043
							;_v@0044 
SetupSize	.EQU SetupPtr+2		;_v@0045

SetupBuf	.EQU SetupSize+1		;_v@0046
;SetupBuf+1	SetupBuf+1 .EQU $0047
;SetupBuf+2 SetupBuf+2 .EQU $0048
;SetupBuf+3 SetupBuf+3 .EQU $0049
;SetupBuf+4 SetupBuf+4 .EQU $004a
;SetupBuf+5 SetupBuf+5 .EQU $004b
;SetupBuf+6 SetupBuf+6 .EQU $004c
;SetupBuf+7
;SetupEnd 	.EQU $004d
SetupLen	.EQU (SuspendCount - SetupBuf)
SuspendCount .EQU SetupBuf+8

_v@004f 	.EQU $004f
_v@0050 	.EQU $0050
_v@0051		.EQU $0051
_v@0055 	.EQU $0055
_v@0061 	.EQU $0061
_v@0062 	.EQU $0062
_v@0063 	.EQU $0063
_v@0064 	.EQU $0064
_v@0065 	.EQU $0065
_v@0069 	.EQU $0069
_v@006b 	.EQU $006b
_v@0070 	.EQU $0070
_v@0072 	.EQU $0072

usbState	.EQU $0073		;_v@0073
KeyState 	.EQU $0074

TimerVal	.EQU $0075
channel		.EQU $0077
Epoch		.EQU $0078

		.IF WITH_TIMER_INT
TimerOvf	.EQU (Epoch + 1)
lastVar		.EQU (TimerOvf + 2)
		.ELSE

		.IF WITH_LOOP_SAFETY > 0
CounterVal	.EQU (Epoch + 1)
lastVar		.EQU (CounterVal + 2)
		.ELSE
lastVar		.EQU (Epoch + 1)
		.ENDIF
		.ENDIF


RamEnd		.EQU lastVar


    .ORG  $dc00


	.MACRO init_sadc
	; disable internal Pull-Ups on PTE                                                     
    ; POCR &= ~0x80;  // disable PTE20P   	
		and_bits	POCR,#$7f
	.ENDM                        ; .     $e13a   81


	.MACRO config_hc08				; fklgjfldg
	;disable COP, disable USB-reset
		mov    #$21,CONFIG              ; ..    $dffa   b7 1f
	; disable COP
;	mov		#1,CONFIG
		clr     TSC                 ; ..    $dffc   3f 0a
;	mov		#$40,TSC
	.IF WITH_TIMER_INT > 0
;	bset.6	TSC		
	.ENDIF
		init_sadc
    	or_bits DDRD,#$07			; init LED Output
	.ENDM

	.MACRO init_keyboard
		or_bits	POCR,#$01
		or_bits	PTA,#$70
		or_bits	DDRA,#$70
		and_bits	DDRA,#$8f
		mov    #$70,KBIER               ; ..    $e0fb   b7 17
		mov    #$04,KBSCR               ; ..    $e0ff   b7 16
		clr 	KeyState
	.ENDM

	.MACRO init_leds		                       ; .     $e105   81
   		ldHX   #DDRD                ; E..   $e006   45 00 07
   		ldA    #$07                 ; ..    $e009   a6 07
   		orA   ,X                   ; .     $e00b   fa
   		stA   ,X                   ; .     $e00c   f7
	.ENDM

	.MACRO 	t_ha
		pshH
		pulA
	.ENDM

	.MACRO start_sadc
	and_bits	DDRD,#$87		;DDRD &= ~0x78;		// PTD[3..6] -> Input
	and_bits DDRE,#$f8		;DDRE &= ~0x07;		// PTE[0..2] -> Input
	and_bits PTD,#$87		;PTD &= ~0x78;
	or_bits PTE,#$07			;PTE |= 0x07;
	.ENDM


_ResetEntry:
	config_hc08

;	ldHX   #$013f               ; E..   $dc00   45 01 3f 
	ldHX   #$0140               ; E..   $dc00   45 01 3f 
	tHXS                         ; .     $dc03   94

   ldHX   #RamBegin            ; E.A   $dc07   45 00 41 
   bra      _Ldc0f            ; ..    $dc0a   20 03
_Ldc0c:
   clr   ,X                   ; .     $dc0c   7f
   aiX    #$01				   ; ..    $dc0d   af 01 
_Ldc0f:
   cpHX   #RamEnd               ; e.u   $dc0f   65 00 75 
   bne      _Ldc0c            ; ..    $dc12   26 f8
   jsr      MainEntry          ; ...   $dc18   cd e0 01 
_StayHere2Die:
   bra      _StayHere2Die      ; ..    $dc1b   20 fe 


ForceStall:
	or_bits	UCR3,#$30
	rts                         ; .     $dc24   81 

F_@dc25:
   ldA.8      SetupBuf+3	;SetupBuf+3            ; ..I   $dc25   c6 00 49 
   bne      _Ldc39            ; ..    $dc28   26 0f
   ldA.8      SetupBuf+5	;SetupBuf+5            ; ..K   $dc2a   c6 00 4b 
   bne      _Ldc39            ; ..    $dc2d   26 0a
   ldA.8      SetupBuf+7	;SetupBuf+7            ; ..M   $dc2f   c6 00 4d 
   bne      _Ldc39            ; ..    $dc32   26 05

   ldA.8      SetupBuf+6	;SetupBuf+6            ; ..L   $dc34   c6 00 4c 
   beq      _Ldc3b            ; ..    $dc37   27 02
_Ldc39:
   bra      ForceStall            ; ..    $dc39   20 e2 
_Ldc3b:
   ldA.8     SetupBuf	;_v@0046            ; ..F   $dc3b   c6 00 46 
   cmp    #$02                 ; ..    $dc3e   a1 02 
   bne      _Ldc39            ; ..    $dc40   26 f7

   ldA.8      SetupBuf+2	;SetupBuf+2            ; ..H   $dc42   c6 00 48 
   bne      _Ldc39            ; ..    $dc45   26 f2

	ldA.8      SetupBuf+4	;SetupBuf+4            ; ..J   $dc47   c6 00 4a 
	cbeqA	$81,_Ldc51        ; A..   $dc4a   41 81 04
	cbeqA	$02,_Ldc5e
;   cmp    #$02                 ; ..    $dc4d   a1 02 
;   bne      _Ldc39            ; ..    $dc4f   26 e8
;	bra		_Ldc39
	bra		ForceStall
_Ldc51:
;   cmp    #$81                 ; ..    $dc51   a1 81 
;   bne      _Ldc5e            ; ..    $dc53   26 09
	and_bits	UCR1,#$3f

   bra      _Ldc69            ; ..    $dc5c   20 0b
_Ldc5e:
	and_bits	UCR2,#$bf
	clr 	R2Seq
_Ldc69:
   mov    #$a0,UCR0                ; ..    $dc6b   b7 3b 
   rts                         ; .     $dc6d   81 

F_@dc6e:
   ldA.8      SetupBuf+3	;SetupBuf+3            ; ..I   $dc82   c6 00 49 
   bne      _Ldc8e            ; ..    $dc85   26 07
   ldA.8      SetupBuf+4	;SetupBuf+4            ; ..J   $dc73   c6 00 4a 
   bne      _Ldc8e            ; ..    $dc76   26 16
   ldA.8      SetupBuf+5	;SetupBuf+5            ; ..K   $dc6e   c6 00 4b 
   bne      _Ldc8e            ; ..    $dc71   26 1b
   ldA.8      SetupBuf+6	;SetupBuf+6            ; ..L   $dc7d   c6 00 4c 
   bne      _Ldc8e            ; ..    $dc80   26 0c
   ldA.8      SetupBuf+7	;SetupBuf+7            ; ..M   $dc78   c6 00 4d 
   bne      _Ldc8e            ; ..    $dc7b   26 11
;	ldX		#SetupBuf+3			;SetupBuf+3
;	ldA		#0
;	cbeqp	,X,_Ldc8e
;	cbeqp	,X,_Ldc8e
;	cbeqp	,X,_Ldc8e
;	cbeqp	,X,_Ldc8e
;	cbeqp	,X,_Ldc8e

   ldA.8      SetupBuf+2	;SetupBuf+2            ; ..H   $dc87   c6 00 48 
   bit    #$80                 ; ..    $dc8a   a5 80 
   beq      _Ldc90            ; ..    $dc8c   27 02
_Ldc8e:
   bra      ForceStall            ; ..    $dc8e   20 8d 
_Ldc90:
   mov    #$a0,UCR0                ; ..    $dc6b   b7 3b 
   rts                         ; .     $dc94   81 

F_@dc95:
   ldA.8      SetupBuf+3            ; ..I   $dca9   c6 00 49 
   bne      _Ldcbc            ; ..    $dcac   26 0e
   ldA.8      SetupBuf+4            ; ..J   $dc9a   c6 00 4a 
   bne      _Ldcbc            ; ..    $dc9d   26 1d
   ldA.8      SetupBuf+5            ; ..K   $dc95   c6 00 4b 
   bne      _Ldcbc            ; ..    $dc98   26 22
   ldA.8      SetupBuf+6            ; ..L   $dca4   c6 00 4c 
   bne      _Ldcbc            ; ..    $dca7   26 13
   ldA.8      SetupBuf+7            ; ..M   $dc9f   c6 00 4d 
   bne      _Ldcbc            ; ..    $dca2   26 18
;	ldX		#SetupBuf+3			;SetupBuf+3
;	ldA		#0
;	cbeqp	,X,_Ldcbc
;	cbeqp	,X,_Ldcbc
;	cbeqp	,X,_Ldcbc
;	cbeqp	,X,_Ldcbc
;	cbeqp	,X,_Ldcbc
	
   ldA.8      SetupBuf+2            ; ..H   $dcae   c6 00 48 
   cmp    #$02                 ; ..    $dcb1   a1 02 
   bhs      _Ldcbc            ; ..    $dcb3   24 07
   ldA.8      usbState            ; ..s   $dcb5   c6 00 73 
   cmp    #$02                 ; ..    $dcb8   a1 02 
   bne      _Ldcbf            ; ..    $dcba   26 03
_Ldcbc:
   jmp      ForceStall            ; ...   $dcbc   cc dc 1d 
_Ldcbf:
   ldA.8      SetupBuf+2            ; ..H   $dcbf   c6 00 48 
   beq      _Ldcd3            ; ..    $dcc2   27 0f
   mov    #$04,usbState          
   mov    #$20,UCR1             
   mov    #$10,UCR2             
   bra      _Ldcdc            ; ..    $dcd1   20 09
_Ldcd3:
   mov    #$03,usbState               ; ..    $dcd3   a6 03 
   clr     UCR1                ; ..    $dcd8   3f 3c 
   clr     UCR2                ; ..    $dcda   3f 19 
_Ldcdc:
   mov    #$a0,UCR0                 ; ..    $dcdc   a6 a0 
   rts                         ; .     $dce0   81 

F_@dce1:
   aiS    #$fd                 ; ..    $dce1   a7 fd 
   ldA.8      SetupBuf+3            ; ..I   $dce3   c6 00 49 
   cbeqA 	$01,_Ldcf4        ; A..   $dce6   41 01 0b
   cbeqA 	$02,_Ldd02        ; A..   $dce9   41 02 16
   cbeqA 	$03,_Ldd10        ; A..   $dcec   41 03 21
   jsr      ForceStall            ; ...   $dcef   cd dc 1d 
   bra      _Ldd27            ; .3    $dcf2   20 33
_Ldcf4:
	ldHX	#_DeviceDesc		;	#$e263
	stHX	SetupPtr
   ldA    #$12                 ; ..    $dcfe   a6 12 
   bra      _Ldd24            ; ..    $dd00   20 22
_Ldd02:
	ldHX	#_ConfigDesc		;	#$e275
	stHX	SetupPtr
   ldA    #$20                 ; ..    $dd0c   a6 20 
   bra      _Ldd24            ; ..    $dd0e   20 14
_Ldd10:
   ldX.8      SetupBuf+2            ; ..H   $dd10   ce 00 48 
   lslX                        ; X     $dd13   58 
   clrH                        ; .     $dd14   8c 
   ldA   	_Str0Vec,X           ; ...   $dd15   d6 e2 ef
   stA.8      SetupPtr            ; ..C   $dd18   c7 00 43 
   ldX   	[_Str0Vec+1],X             ; ...   $dd1b   de e2 f0
   stX.8      SetupPtr+1            ; ..D   $dd1e   cf 00 44 
   pshA                        ; .     $dd21   87 
   pulH                        ; .     $dd22   8a 
   ldA   ,X                   ; .     $dd23   f6
_Ldd24:
   stA.8      SetupSize            ; ..E   $dd24   c7 00 45 
_Ldd27:
   ldA.8      SetupBuf+3            ; ..I   $dd27   c6 00 49 
   cbeqA	$01,_Ldd34        ; A..   $dd2a   41 01 07
   cbeqA	$02,_Ldd34        ; A..   $dd2d   41 02 04
   cmp   	 #$03                 ; ..    $dd30   a1 03
   bne      _Ldd91            ; ..    $dd32   26 5d
_Ldd34:
   ldA.8      SetupBuf+6            ; ..L   $dd34   c6 00 4c 
   cmp.8      SetupSize            ; ..E   $dd37   c1 00 45 
   bhs      _Ldd47            ; ..    $dd3a   24 0b
   ldA.8      SetupBuf+7            ; ..M   $dd3c   c6 00 4d 
   bne      _Ldd47            ; ..    $dd3f   26 06
   mov      SetupBuf+6,SetupSize            ; ..E   $dd44   c7 00 45 
_Ldd47:
   tSHX                         ; .     $dd47   95
   clr   ,X                   ; .     $dd48   7f
   ldA    #$20                 ; ..    $dd49   a6 20 
   stA.8 	$02,X               ; ..    $dd4b   e7 02
   clr   	$01,X               ; o.    $dd4d   6f 01
   bra      _Ldd78            ; ..    $dd4f   20 27
_Ldd51:
	ldHX	SetupPtr
   ldA   ,X                   ; .     $dd59   f6
   tSHX                         ; .     $dd5a   95
   ldX.8 $01,X              ; ..    $dd5b   ee 01
   pshX                        ; .     $dd5d   89 
   ldX.8 $04,SP              ; ...   $dd5e   9e ee 04
   pulH                        ; .     $dd61   8a 
   stA   ,X                   ; .     $dd62   f7
   tSHX                         ; .     $dd63   95
   inc   $02,X              ; l.    $dd64   6c 02
   bne      _Ldd6a            ; ..    $dd66   26 02
   inc   $01,X              ; l.    $dd68   6c 01
_Ldd6a:
   ldHX   #SetupPtr             ; E.C   $dd6a   45 00 43 
   inc   $01,X              ; l.    $dd6d   6c 01
   bne      _Ldd72            ; ..    $dd6f   26 01
   inc   ,X                   ; .     $dd71   7c
_Ldd72:
   ldHX   #SetupSize             ; E.E   $dd72   45 00 45 
   dec   ,X                   ; z     $dd75   7a
   tSHX                         ; .     $dd76   95
   inc   ,X                   ; .     $dd77   7c
_Ldd78:
   ldA.8      SetupSize            ; ..E   $dd78   c6 00 45 
   beq      _Ldd82            ; ..    $dd7b   27 05
   ldA   ,X                   ; .     $dd7d   f6
   cmp    #$08                 ; ..    $dd7e   a1 08 
   blo      _Ldd51            ; ..    $dd80   25 cf
_Ldd82:
   ldA   ,X                   ; .     $dd82   f6
   add    #$a0                 ; ..    $dd83   ab a0 
   stA.8   UCR0                ; ..    $dd85   b7 3b 
   ldA   ,X                   ; .     $dd87   f6
   cmp    #$08                 ; ..    $dd88   a1 08 
   bhs      _Ldd91            ; ..    $dd8a   24 05
   mov    #$ff,SetupBuf+1            ; ..G   $dd8e   c7 00 47 
_Ldd91:
   aiS    #$03                 ; ..    $dd91   a7 03 
   rts                         ; .     $dd93   81 

F_@dd94:
	and_bits	UCR0,#$ef
	mov    #$01,UIR2                ; ..    $dd9d   b7 18 

	ldHX    #SetupLen                 ; ..    $dda0   ae 08 
_Ldda2:
	ldA.8 	[UE0D0-1],X               ; ..    $dda2   e6 1f
	stA.8	[SetupBuf-1],X
	dbnzX    _Ldda2            ; ..    $dda7   5b f9

   ldA.8   USR0                ; ..    $dda9   b6 3d 
   cbeqA	$48,  _Lddb3        ; AH.   $ddab   41 48 05
   jsr      ForceStall            ; ...   $ddae   cd dc 1d 
   bra      _Lddd6            ; ..    $ddb1   20 23
_Lddb3:
   ldA.8      SetupBuf            ; ..F   $ddb3   c6 00 46 
   bit    #$60                 ; ..    $ddb6   a5 60 
   beq      _Lddbf            ; ..    $ddb8   27 05
   jsr      ForceStall            ; ...   $ddba   cd dc 1d 
   bra      _Lddd6            ; ..    $ddbd   20 17
_Lddbf:
   ldA.8      SetupBuf+1            ; ..G   $ddbf   c6 00 47 
   cbeqA 	$01,  _Lddd3        ; A..   $ddc2   41 01 0e
   cbeqA 	$05,  _Lddde        ; A..   $ddc5   41 05 16
   cbeqA 	$06,  _Ldde3        ; A..   $ddc8   41 06 18
   cbeqA 	$09,  _Ldde8        ; A..   $ddcb   41 09 1a
   jsr      ForceStall            ; ...   $ddce   cd dc 1d 
   bra      _Lddd6            ; ..    $ddd1   20 03
_Lddd3:
   jsr      F_@dc25            ; ...   $ddd3   cd dc 25 
_Lddd6:
	or_bits	UCR0,#$10
   rts                         ; .     $dddd   81 
_Lddde:
   jsr      F_@dc6e            ; ..n   $ddde   cd dc 6e 
   bra      _Lddd6            ; ..    $dde1   20 f3
_Ldde3:
   jsr      F_@dce1            ; ...   $dde3   cd dc e1 
   bra      _Lddd6            ; ..    $dde6   20 ee
_Ldde8:
   jsr      F_@dc95            ; ...   $dde8   cd dc 95 
   bra      _Lddd6            ; ..    $ddeb   20 e9

F_@dded:
	and_bits UCR0,#$cf
	mov    #$01,UIR2                ; ..    $ddf6   b7 18 
	or_bits UCR0,#$10
	rts                         ; .     $ddfc   81 

F_@ddfd:
	aiS    #$fd                 ; ..    $ddfd   a7 fd 
	and_bits UCR0,#$df
	mov    #$02,UIR2                ; ..    $de08   b7 18 
	ldA.8      SetupBuf+1            ; ..G   $de0a   c6 00 47 
	cbeqA 	$01,_Lde85            ; .t    $de0f   27 74
	cbeqA 	$05,_Lde23        ; A..   $de11   41 05 0f
	cbeqA 	$06,_Lde3a        ; A..   $de14   41 06 23
	cbeqA   $09,_Lde85            ; .j    $de19   27 6a
	incA                        ; L     $de1b   4c 
	beq      _Lde8a            ; .l    $de1c   27 6c
	jsr      ForceStall            ; ...   $de1e   cd dc 1d 
	bra      _Lde8a            ; .g    $de21   20 67
_Lde23:
	ldA.8      SetupBuf+2            ; ..H   $de23   c6 00 48 
	orA    #$80                 ; ..    $de26   aa 80 
	stA.8   UADDR               ; .8    $de28   b7 38 
   ldA.8      SetupBuf+2            ; ..H   $de2a   c6 00 48 
   beq      _Lde33            ; ..    $de2d   27 04
   ldA    #$03                 ; ..    $de2f   a6 03 
   bra      _Lde35            ; ..    $de31   20 02
_Lde33:
   ldA    #$02                 ; ..    $de33   a6 02 
_Lde35:
   stA.8      usbState            ; ..s   $de35   c7 00 73 
   bra      _Lde85            ; .K    $de38   20 4b
_Lde3a:
   tSHX                         ; .     $de3a   95
   clr   ,X                   ; .     $de3b   7f
   ldA    #$20                 ; ..    $de3c   a6 20 
   stA.8 $02,X              ; ..    $de3e   e7 02
   clr   $01,X              ; o.    $de40   6f 01
   bra      _Lde6b            ; ..    $de42   20 27
_Lde44:
	ldHX	SetupPtr 
   ldA   ,X                   ; .     $de4c   f6
   tSHX                         ; .     $de4d   95
   ldX.8 $01,X              ; ..    $de4e   ee 01
   pshX                        ; .     $de50   89 
   ldX.8 $04,SP              ; ...   $de51   9e ee 04
   pulH                        ; .     $de54   8a 
   stA   ,X                   ; .     $de55   f7
   tSHX                         ; .     $de56   95
   inc   $02,X              ; l.    $de57   6c 02
   bne      _Lde5d            ; ..    $de59   26 02
   inc   $01,X              ; l.    $de5b   6c 01
_Lde5d:
   ldHX   #SetupPtr             ; E.C   $de5d   45 00 43 
   inc   $01,X              ; l.    $de60   6c 01
   bne      _Lde65            ; ..    $de62   26 01
   inc   ,X                   ; .     $de64   7c
_Lde65:
   ldHX   #SetupSize             ; E.E   $de65   45 00 45 
   dec   ,X                   ; z     $de68   7a
   tSHX                         ; .     $de69   95
   inc   ,X                   ; .     $de6a   7c
_Lde6b:
   ldA.8      SetupSize            ; ..E   $de6b   c6 00 45 
   beq      _Lde75            ; ..    $de6e   27 05
   ldA   ,X                   ; .     $de70   f6
   cmp    #$08                 ; ..    $de71   a1 08 
   blo      _Lde44            ; ..    $de73   25 cf
_Lde75:
   ldA.8   UCR0                ; ..    $de75   b6 3b 
   eor    #$80                 ; ..    $de77   a8 80 
   and    #$80                 ; ..    $de79   a4 80 
   add   ,X                   ; .     $de7b   fb
   add    #$30                 ; .0    $de7c   ab 30 
   stA.8   UCR0                ; ..    $de7e   b7 3b 
   ldA   ,X                   ; .     $de80   f6
   cmp    #$08                 ; ..    $de81   a1 08 
   bhs      _Lde8a            ; ..    $de83   24 05
_Lde85:
	mov		#$ff,SetupBuf+1
_Lde8a:
   aiS    #$03                 ; ..    $de8a   a7 03 
   rts                         ; .     $de8c   81 

F_@de8d:
   aiS    #$fd                 ; ..    $de8d   a7 fd 
	and_bits	UCR1,#$df
   mov    #$08,UIR2                ; ..    $de98   b7 18 
   tSHX                         ; .     $de9a   95
   clr   ,X                   ; .     $de9b   7f
   ldA    #$28                 ; ..    $de9c   a6 28 
   stA.8 $02,X              ; ..    $de9e   e7 02
   clr   $01,X              ; o.    $dea0   6f 01
   bra      _Ldec5            ; ..    $dea2   20 21
_Ldea4:
   ldX.8      _v@0062            ; ..b   $dea4   ce 00 62 
   clrH                        ; .     $dea7   8c 
   ldA.8   _v@0063,X             ; ..c   $dea8   d6 00 63
   tSHX                         ; .     $deab   95
   ldX.8 $01,X              ; ..    $deac   ee 01
   pshX                        ; .     $deae   89 
   ldX.8 $04,SP              ; ...   $deaf   9e ee 04
   pulH                        ; .     $deb2   8a 
   stA   ,X                   ; .     $deb3   f7

   ldA.8      _v@0062            ; ..b   $deb4   c6 00 62 
   incA                        ; L     $deb7   4c 
   and    #$0f                 ; ..    $deb8   a4 0f 
   stA.8      _v@0062            ; ..b   $deba   c7 00 62 

   tSHX                         ; .     $debd   95
   inc   $02,X              ; l.    $debe   6c 02
   bne      _Ldec4            ; ..    $dec0   26 02
   inc   $01,X              ; l.    $dec2   6c 01
_Ldec4:
   inc   ,X                   ; .     $dec4   7c
_Ldec5:
   ldA.8      _v@0062            ; ..b   $dec5   c6 00 62 
   cmp.8      _v@0061            ; ..a   $dec8   c1 00 61 
   beq      _Lded2            ; ..    $decb   27 05
   ldA   ,X                   ; .     $decd   f6
   cmp    #$08                 ; ..    $dece   a1 08 
   blo      _Ldea4            ; ..    $ded0   25 d2
_Lded2:
   ldA.8   UCR1                ; ..    $ded2   b6 3c 
   eor    #$80                 ; ..    $ded4   a8 80 
   and    #$80                 ; ..    $ded6   a4 80 
   add   ,X                   ; .     $ded8   fb
   add    #$20                 ; ..    $ded9   ab 20 
   stA.8   UCR1                ; ..    $dedb   b7 3c 
   aiS    #$03                 ; ..    $dedd   a7 03 
   rts                         ; .     $dedf   81 

F_@dee0:
	aiS    #$fc                 ; ..    $dee0   a7 fc 

	and_bits UCR2,#$ef
	mov    #$10,UIR2                ; ..    $deeb   b7 18 

	ldA    #$30                 ; .0    $deed   a6 30 
	tSHX                         ; .     $deef   95
	stA.8 3,X               ; ..    $def0   e7 03
	clr   $02,X              ; o.    $def2   6f 02
	ldA.8   USR1                ; ..    $def4   b6 3e 
	and    #$0f                 ; ..    $def6   a4 0f 
	stA.8 $01,X              ; ..    $def8   e7 01
	bra      _Ldf24            ; ..    $defa   20 28
_Ldefc:
   ldA.8      _v@004f            ; ..O   $defc   c6 00 4f 
   incA                        ; L     $deff   4c 
   and    #$0f                 ; ..    $df00   a4 0f 
   stA   ,X                   ; .     $df02   f7
_Ldf03:
   cmp.8      _v@0050            ; ..P   $df03   c1 00 50 
   beq      _Ldf03            ; ..    $df06   27 fb
   tSHX                         ; .     $df08   95
   ldA.8 $02,X              ; ..    $df09   e6 02
   ldX.8 3,X               ; ..    $df0b   ee 03
   pshA                        ; .     $df0d   87 
   pulH                        ; .     $df0e   8a 
   ldA   ,X                   ; .     $df0f   f6
   ldX.8      _v@004f            ; ..O   $df10   ce 00 4f 
   clrH                        ; .     $df13   8c 
   stA.8   $0051,X             ; ..Q   $df14   d7 00 51
   tSHX                         ; .     $df17   95
   ldA   ,X                   ; .     $df18   f6
   stA.8      _v@004f            ; ..O   $df19   c7 00 4f 
   inc   3,X               ; l.    $df1c   6c 03
   bne      _Ldf22            ; ..    $df1e   26 02
   inc   $02,X              ; l.    $df20   6c 02
_Ldf22:
   dec   $01,X              ; j.    $df22   6a 01
_Ldf24:
   tst   $01,X              ; m.    $df24   6d 01
   bne      _Ldefc            ; ..    $df26   26 d4
   mov    #$10,UCR2                ; ..    $deeb   b7 18 
   aiS    #$04                 ; ..    $df2c   a7 04 
   rts                         ; .     $df2e   81 

InitUSB:
	mov    #$80,UADDR               ; .8    $df31   b7 38 
	clr     UCR0                ; ..    $df33   3f 3b 
	clr     UCR1                ; ..    $df35   3f 3c 
	clr     UCR2                ; ..    $df37   3f 19 
	mov    #$44,UCR3                ; ..    $df3b   b7 1a 
	clr     UCR4                ; ..    $df3d   3f 1b 
	clr     UIR0                ; .9    $df3f   3f 39 
	mov    #$ff,UIR2                ; ..    $df43   b7 18 
	clr      R0Seq            ; ..B   $df46   c7 00 42 
	clr  	 R2Seq	 		   ; ..A   $df49   c7 00 41
	clr  	_v@0062 		   ; ..b   $df51   c7 00 62
	clr  	_v@0061 		   ; ..a   $df54   c7 00 61
	clr  	_v@0050 		   ; ..P   $df57   c7 00 50
	clr  	_v@004f 		   ; ..O   $df5a   c7 00 4f
	mov 	#1,usbState
	rts                         ; .     $df5d   81 

;F_@df5e:
;ReadUSB:
	.MACRO read_usb
		pshX
		pshH
WaitReceived:
   		ldA.8      _v@0050            ; ..P   $df5f   c6 00 50
   		cmp.8      _v@004f            ; ..O   $df62   c1 00 4f
   		beq      WaitReceived            ; ..    $df65   27 f8
	; load Index register
   		ldX.8      _v@0050            ; ..P   $df67   ce 00 50
   		clrH                        ; .     $df6a   8c
	; increment ReadIdx in Ringbuffer
   		ldA.8      _v@0050            ; ..P   $df70   c6 00 50
   		incA                        ; L     $df73   4c
   		and    #$0f                 ; ..    $df74   a4 0f
   		stA.8      _v@0050            ; ..P   $df76   c7 00 50
	; load element
   		ldA.8   _v@0051,X             ; ..Q   $df6b   d6 00 51
		pulH
		pulX
	.ENDM
;   rts                         ; .     $df7b   81 

;F_@df7c:
;WriteUSB:

	.MACRO write_usb
		pshH                        ; .     $df7c   87
   		pshX                        ; .     $df7d   89
   		pshA                        ; .     $df7e   87

   		ldA.8      _v@0061            ; ..a   $df7f   c6 00 61
   		incA                        ; L     $df82   4c
   		and    #$0f                 ; ..    $df83   a4 0f
;   tSHX                         ; .     $df85   95
;   stA   ,X                   ; .     $df86   f7
		pshA
WaitSended:
   		cmp.8      _v@0062            ; ..b   $df87   c1 00 62
   		beq      WaitSended            ; ..    $df8a   27 fb
;   ldA.8 $02,X              ; ..    $df8c   e6 02
		ldA.8	2,SP

	; load Index register
   		ldX.8      _v@0061            ; ..a   $df8e   ce 00 61
   		clrH                        ; .     $df91   8c
	; insert value into Ringbuffer
   		stA.8   _v@0063,X             ; ..c   $df92   d7 00 63

;   tSHX                         ; .     $df95   95
;   ldA   ,X                   ; .     $df96   f6
		pulA
   		stA.8      _v@0061            ; ..a   $df97   c7 00 61

;   aiS    #$03                 ; ..    $df9a   a7 03 
		pulA		; increment Stack Pointer
		pulX
		pulH
	.ENDM
;   rts                         ; .     $df9c   81 

_USBISR:
	pshH                        ; .     $df9d   8b 
;	pshX
;	pshA

	brclr	7,UIR1, _Ldfae      ; ...   $dfa1   0f 3a 0a
	clr		SuspendCount
	mov    #$80,UIR2                ; ..    $dfaa   b7 18 
	bra      _exitISR            ; .E    $dfac   20 45 
_Ldfae:
	brclr	0, UIR1, _Ldfbe      ; ...   $dfae   01 3a 0d
	brclr	6, USR0, _Ldfb9      ; ...   $dfb1   0d 3d 05
	jsr      F_@dd94            ; ...   $dfb4   cd dd 94 
	bra      _exitISR            ; ..    $dfb7   20 3a 
_Ldfb9:
	jsr      F_@dded            ; ...   $dfb9   cd dd ed 
	bra      _exitISR            ; .5    $dfbc   20 35 
_Ldfbe:
	brclr	1,UIR1, _Ldfc6      ; ...   $dfbe   03 3a 05
	jsr      F_@ddfd            ; ...   $dfc1   cd dd fd 
	bra      _exitISR            ; ..    $dfc4   20 2d 
_Ldfc6:
	brclr	3,UIR1, _Ldfce      ; ...   $dfc6   07 3a 05
	jsr      F_@de8d            ; ...   $dfc9   cd de 8d 
	bra      _exitISR            ; ..    $dfcc   20 25 
_Ldfce:
	brclr	4,UIR1, _Ldfd6      ; ...   $dfce   09 3a 05
	jsr      F_@dee0            ; ...   $dfd1   cd de e0 
	bra      _exitISR            ; ..    $dfd4   20 1d 
_Ldfd6:
	brclr	6, UIR1, _exitISR      ; ...   $dfd6   0d 3a 1a
	jsr      InitUSB            ; ...   $dfd9   cd df 2f 
	or_bits	UCR3,#$03
	mov    #$9b,UIR0                ; .9    $dfe5   b7 39 
	or_bits	UCR0,#$10
	mov    #$02,usbState            ; ..s   $dff0   c7 00 73 
_exitISR:
;	pulA
;	pulX
	pulH                        ; .     $dff6   8a 
	rti                         ; .     $dff7   80 

_TimerOvrISR:
	.IF WITH_TIMER_INT  ;ejwlkq
;	lda.8	TSC
		bclr	7,TSC
		inc		TimerOvf
	.ENDIF
_DummyISR:
	rti                         ; .     $e000   80 

	.MACRO inc_h		; comment
		pshH                        ; .     $e020   8b
		inc   1,SP              ; .l.   $e021   9e 6c 01
		pulH                        ; .     $e024   8a
	.ENDM

	.MACRO add_hx	;	opr	; comment
		tXA
		add		{1}
		tAX
		bhs _LaHX
		incH
_LaHX:
	.ENDM

MainEntry:
	aiS    #$f3                 ; ..    $e001   a7 f3 
	jsr      InitUSB            ; ...   $e003   cd df 2f 
	init_keyboard
	init_leds
	start_sadc
   cli                         ; .     $e013   9a 
   tSHX                         ; .     $e014   95
   clr   9,X               ; o.    $e015   6f 09
_MainLoop:

;~~~begin(Read_USB(8))~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	clrA
	tSHX
	stA.8	8,X		; same as 9,SP
ReadUSBLoop:
	inc		9,SP
	read_usb
	stA		,X
	incX
	ldA.8 	9,SP
 	cmp    	#$08                 ; ..    $e05c   a1 08
	blo      ReadUSBLoop           ; ..    $e05e   25 e3 
;___endof(Read_USB(8))____________________________________________


;~~~begin(LED)~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;	or_bits	DDRD,#$07
	tSHX
	ldA		#$1
LEDLoop:
	pshA
	and		1,X				; 2,SP
	bne		ClearBit
	ldA.8	PTD
	orA		1,SP
	bra 	SetBit
ClearBit:
	ldA.8	1,SP
	comA
	pshA
	ldA.8	PTD
	and.8	1,SP
	aiS		#1
SetBit:
	stA.8	PTD
	pulA
	lslA
	cmp		#8
	blo		LEDLoop
;___endof(LED)________________________________


	ldA.8	3,SP
	cmp		#$AD
	beq		getSADCVal
	jmp		SkipADC

getSADCVal:
	ldA.8	channel

	lsrA
	incA
	pshA		
	ldA		#1
_InBits:
	dec		1,SP
	beq		_OutBits
	lslA	
	bra 	_InBits
_OutBits:
	stA		1,SP		; Number{0,1,2} -> Bit[0..2] stays pushed
	
;	and		#2
;	bne		_channel1
;	lda.8	channel
;	and		#4
;	beq		_channel2
;	lda		#1
;	bra		_channelOut
;_channel2: 
;	lda		#4
;	bra		_channelOut
;_channel1:
;	lda		#2
;_channelOut:
;	pshA					; push channel

;~~~begin(getSADC)~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;
	ldA.8	channel
	and		#1
	bne		_read
	;	DDRD |= 0x78;		// Output L on PTD[3..6]
	or_bits	DDRD,#$78
	bra		_continueADC	
_read:			
;	measure
	;	DDRD &= ~0x38;		// PTD[3..5] = HiZ, PTD[6] = L
	and_bits	DDRD,#$c7
_continueADC:

	;	PTD &= ~0x78;		// all PTD[3..6] = L
	and_bits	PTD,#$87
	;	PTE |= 0x07;		// all PTE[0..2] = H;
	or_bits PTE,#$07
	;	DDRE |= 0x07;		// Output H on PTE[0..2]
	or_bits DDRE,#$07
	
	;	delay025ms();		// discharge C13..C15
		ldA #240
_delay025ms:				; discharge C13..C15
		dbnzA _delay025ms		;	 3cyc x 333ns x 240+9 = 0,249ms

	;	DDRE &= ~0x07;		// PTE[0..2] HiZ (Input)
		and_bits	DDRE,#$f8
		
	;	TimerVal = TCNT;		// cal start time

	.IF WITH_TIMER_INT
		ldA.8	TimerOvf
		stA.8	5,SP
	.ENDIF
	
		ldHX	TCNT
		stHX	TimerVal
	.IF	 WITH_LOOP_SAFETY > 0
		ldHX	#$0000
	.ENDIF
		bclr	5,TSC			; clear TSTOP flag

;	while((PTE & channel) != 0) ;

	ldA.8	1,SP
_ADCLoop:	
	and.8	PTE				;[3]
	.IF WITH_LOOP_SAFETY == 0
		bne		_ADCLoop		;[3]
	.ELSE
		beq		_outADCLoop		;[3]		  	 break;
		aiX		#$0001			;[2]
		cpHX	#$1fff			;[3]
		bhs		_outADCLoop		;[3]
		bra _ADCLoop			;[3]
	.ENDIF
_outADCLoop:
	.IF WITH_LOOP_SAFETY > 0
		stHX	CounterVal
	.ENDIF
		ldHX	TCNT
		tXA
		sub.8	TimerVal+1
		stA.8	7,SP
		t_ha
		sbc.8	TimerVal
		stA.8	6,SP
	.IF WITH_TIMER_INT > 0
		ldA.8	TimerOvf
		sub.8	5,SP
		stA.8	5,SP
	.ELSE
		.IF WITH_LOOP_SAFETY > 0
			ldHX	CounterVal
			t_ha
			sta.8	5,SP
		.ENDIF
	.ENDIF
	
;___endof(getSADC)____________________________________________________
;
	pulA

SkipADC:

;~~~begin(CheckSum)~~~~~~~~~~~~~~~~~~~~~~~~~
	clrA	
	tSHX
	movX+	Epoch		;1,SP
	add		Epoch
	
	movX+	KeyState	;2,SP
	add		KeyState
	.IF WITH_TIMER_INT > 0
		movX+	TimerOvf	;[SP,3]
		add		TimerOvf
	.ELSE
		movX+	channel
		add		channel
	.ENDIF

	add		,X			;[SP,4]
	add		1,X			;5,SP
	add		2,X			;6,SP
	add		3,X			;7,SP
	comA
	stA.8	4,X			;[SP,8]
;___endof(CheckSum)_________________________

;;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	tSHX
	clrA
   	stA.8 $08,X	; same as 9,SP             ; ..    $e0c1   e7 08
WriteUSBLoop:
	inc   $09,SP               ; l.    $e0c3   6c 08
	ldA   ,X       ; 1,SP ..[SP,8]            ; .     $e0d1   f6
	write_usb
	incX
	ldA.8	9,SP
	cmp    #$08                 ; ..    $e0d9   a1 08 
	blo      WriteUSBLoop            ; ..    $e0db   25 e6 

;	tSHX
;;__________________________________________________________

	inc		Epoch

	ldA.8	channel
	incA
	cmp		#6
	blo		_nextC
	ldA		#0
_nextC:
	stA.8	channel

   jmp      _MainLoop			;_Le017            ; ...   $e0dd   cc e0 17





;F_@e106:
;GetKey:
;   pshA                        ; .     $e106   87 
;   pshX                        ; .     $e107   89 
;   ldA    #$03                 ; ..    $e108   a6 03 
;   tSHX                         ; .     $e10a   95
;   add.8 $01,X              ; ..    $e10b   eb 01
;   stA.8 $01,X              ; ..    $e10d   e7 01
;   ldA    #$01                 ; ..    $e10f   a6 01 
;   ldX.8 $01,X              ; ..    $e111   ee 01
;   beq      _Le118            ; ..    $e113   27 03
;_Le115:
;   lslA                        ; H     $e115   48 
;   dbnzX    _Le115            ; ..    $e116   5b fd
;_Le118:
;   and.8      KeyState            ; ..t   $e118   c4 00 74 
;   beq      _Le11f            ; ..    $e11b   27 02
;   ldA    #$01                 ; ..    $e11d   a6 01 
;_Le11f:
;   aiS    #$02                 ; ..    $e11f   a7 02 
;   rts                         ; .     $e121   81 

_KeyboardISR:
;	pshA
;	pshX
   pshH                        ; .     $e122   8b 
   ldA.8   PTA                 ; ..    $e123   b6 00 
   orA    #$8f                 ; ..    $e125   aa 8f 
   comA                        ; C     $e127   43 
   ldHX   #KeyState             ; E.t   $e128   45 00 74 
   eor   ,X                   ; .     $e12b   f8
   stA   ,X                   ; .     $e12c   f7
   mov    #$04,KBSCR               ; ..    $e0ff   b7 16 
   pulH 
;	pulX
;	pulA                       ; .     $e131   8a 
   rti                         ; .     $e132   80 

;2,0x40		42
;2,0x10		12
;4,0x40		44	
;4,0x20		42

;push dummy
;push ~bit
;push channel
;jsr
;	rts

	.ORG	$e263

_DeviceDesc:	
	.DC.b   	$12,$01,$10,$01,$00,$00,$00,$08,$70
	.DC.b		$0c,$00,$00,$00,$01,$01,$02,$00,$01

_ConfigDesc:
	.DC.b		$09,$02,$20,$00,$01,$01,$00,$c0,$00
_IfaceDesc:
	.DC.b		$09,$04,$00,$00,$02,$ff,$01,$ff,$00
_EP0Desc:
	.DC.b		$07,$05,$81,$03,$08,$00,$0a
_EP1Desc:
	.DC.b		$07,$05,$02,$03,$08,$00,$0a

_Str0Desc:
	.DC.b 	_Str1Desc - _Str0Desc,$03
	.DC.b  	$09,$04
_Str1Desc:
	.DC.b   	_Str2Desc - _Str1Desc,$03		; $28,$03
	.DC.w		"MCT Electronikladen"
_Str2Desc:
	.DC.b		_StrTable - _Str2Desc,$03		; $2e,$03
	.DC.w		"USB08 Evaluation Board"
_StrTable:
_Str0Vec	.DC.w	_Str0Desc	; ..    $e2ef   e2 95
_Str1Vec	.DC.w	_Str1Desc	; ..    $e2f1   e2 99
_Str2Vec	.DC.w	_Str2Desc	; ..    $e2f3   e2 c1


_vec1:		;     /* Keypad			   */
    	declare_vector	_vect_KBIR,  _KeyboardISR
_vec2:    	;     /* TIMER overflow	   */
    	declare_vector	_vect_TIMOVF,_TimerOvrISR
_vec3:    	;     /* TIMER channel 1	   */
    	declare_vector	_vect_TIM1,  _DummyISR
_vec4:    	;     /* TIMER channel 0	   */
    	declare_vector	_vect_TIM0,  _DummyISR
_vec5:    	;     /* IRQ1  			   */
    	declare_vector	_vect_IRQ,   _DummyISR
_vec6:  	;     /* USB					*/
    	declare_vector	_vect_USB,   _USBISR
_vec7:   	;     /* SWI					*/
    	declare_vector	_vect_SWI,	 _DummyISR
_vec8:    	;     /* RESET 			   */
    	declare_vector	_vect_RESET, _ResetEntry


 	.END
 
.
