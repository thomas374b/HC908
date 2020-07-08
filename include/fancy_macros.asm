




	.MACRO  alignspace
_alMod{1}       .EQU    ({2}-(. % {2}))
_alSpace{1}     .DS     _alMod{1},$ac
	.ENDM
