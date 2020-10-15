I used the DASM assembler in all my HC908 projects. It can be found here https://github.com/dasm-assembler/dasm. Assembler source files may not be compatible with assemblers from other vendors. 

I used to write mnemonics in camel-case. This is to get a better visual understanding 
when a mnemonic states an implicit operand (register). So I choosed to write the 
operation lowercase and the operand in uppercase. It works well because DASM is case 
insensitive but be aware of it when you state your own definitions.

There are only two mnemonic-addressmode combinations that are not cleanly supported. 
One has to use a special dasm syntax (forced address-mode extension) to trick dasm 
to create the correct opcodes. But it doesn't harm if you don't. Only 1 byte extra 
space would be needed, the semantic is kept the same. See test/suite68hc908.asm for 
details.

