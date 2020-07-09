
;
;	MACRO declaration keywords _must_ be uppercase
;   MACRO names _must_ be all lowercase
;	MACROS may be nested
;


	.MACRO  declare_vector
		.ORG  	{1}
		.DC.w	{2}
	.ENDM


	.MACRO declare_var
{1}			.EQU		next_var
next_var	.SET		({1}+{2})
	.ENDM

;next_var<------>.SET<-->RAMBASE


	.MACRO	store_reg
		.IF ({1} > 255)
			ldA     {2}		;[2]
			stA		{1}		;[3]
		.ELSE
			mov 	{2},{1}
		.ENDIF
	.ENDM

	.MACRO	store_reg16
			ldHX     {2}		;[2]
			stHX     {1}		;[3]
	.ENDM

	.MACRO	set_bits
            ldHX    #{1}
		    ldA		,X
            orA		{2}
            stA		,X
	.ENDM

	.MACRO	clear_bits
;_NOT_{2}	.EQU	((~{2}) & $FF)
			ldHX	#{1}
			ldA		,x
;			and		#_NOT_{2}
			and		#[[~{2}] & $FF]
			stA		,x
	.ENDM


	.MACRO	shake_pin			; pin, port, label-name infix
		brset	{1},{2},_{3}off
_{3}on:
		bset	{1},{2}
		bra		_after{3}
_{3}off:
		bclr	{1},{2}
_after{3}:
	.ENDM


	.MACRO busy_loop_delay
  			ldA	{1}				; [2]
_L1_{2}:
  			dbnzA	_L1_{2}		; [3] 3*(arg-1) + 2 T
  	.ENDM

	.MACRO and_bits
   		ldA.8  {1}                    ; .     $dc20   f6
   		and    {2}                 ; .0    $dc21   aa 30
   		stA.8  {1}                    ; .     $dc23   f7
	.ENDM


	.MACRO or_bits			        ;hfjdkghkd
   		ldA.8  {1}                    ; .     $dc20   f6
   		orA    {2}                 ; .0    $dc21   aa 30
   		stA.8  {1}                    ; .     $dc23   f7
	.ENDM






