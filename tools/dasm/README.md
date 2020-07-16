I used the DASM assembler in all my HC908 projects. Assembler source files may not be compatible with other assemblers. Especcially
since I incorporated some extra features. As long as they are not available in the upstream version please use my fork of DASM assembler 
https://github.com/thomas374b/dasm.

My version consists of the following changes

* support for mc68hc908 controller family
* stack-pointer indexed address modi added
* allow at-sign (@) to apear in label names
* allow '0x' prefixed hexadezimal notation, example '0x57' for '$57'  
* Bugfixes

I used to write mnemonics in camel-case. This is to get a better visual understanding 
when a mnemonic states an implicit operand (register). So I choosed to write the 
operation lowercase and the operand in uppercase. It works well because DASM is case 
insensitive but be aware of it when you state your own definitions.

examples:
 ldA	#6	; load value 6 to register 'A'
 stHX   5	; store content of register 'HX' to location @ address 5
 pulA	    ; pull register A from stack

Only two mnemonic-addressmode combinations are not yet implemented since it would require 
more difficult changes for the DASM engine. See test/suite68hc908.asm for details.

