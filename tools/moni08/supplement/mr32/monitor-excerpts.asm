;/*
; * monitor-excerpts.asm
; *
; *  Created on: 17.06.2020
; *      Author: pantec
; */


; cycles = (((A-3)*3 +10)*X) + 7
; longest wait time 25.1 s
MoniRomDelayLoop:
   decA                       ; J     $fd21   4a
_Lfd22:
   pshA                       ; .     $fd22   87
   decA                       ; J     $fd23   4a
   decA                       ; J     $fd24   4a
_Lfd25:
   dbnzA    _Lfd25            ; K.    $fd25   4b fe
   pulA                       ; .     $fd27   86
   dbnzX    _Lfd22            ; ..    $fd28   5b f8
   rts                        ; .     $fd2a   81

