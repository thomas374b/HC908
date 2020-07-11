Here you find my patches to the famous DASM assembler version 2.20.14-SNAPSHOT.
    https://github.com/dasm-assembler/dasm

It consists of the following changes

* support for mc68hc908 controller family
* stack-pointer indexed address modi added
* larger hash-table size to avoid clashes when assembling projects with lots of symbols
* more strict checking of upper-/lowercase spelling to avoid clashes in symbols
* allow at-sign (@) to apear in label names
* allow '0x' prefixed hexadezimal notation, example '0x57' for '$57'  
* 2 bug workarounds

The mnemonics of the 68hc908 are written in camel-case (as I do in my code examples). 
I wanted to have a better visual understanding when a mnemonic states an implicit
operand (register). So I choosed to write the operation lowercase and the operand 
in uppercase. 

examples:
    ldA	   #6	; load value 6 to register 'A'
    stHX   5	; store content of register 'HX' to location @ address 5
    pulA	; pull register A from stack

Only two mnemonic-addressmode combinations are not yet implemented since it would require 
more difficult chenges for the DASM engine. See test/suite68hc908.asm for details.


You can find my forked version of dasm also on github 
    https://github.com/thomas374b/dasm
