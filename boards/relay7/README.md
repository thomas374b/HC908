Here you find files for my simple power switching application that is assembled
form spare parts of different sources.

One can switch mains power on 7 channels with relays connected 
via an ULN line driver to a hc908jk3.

The 3x2 keypad is recycled from a Saeco coffee machine. The keypad is
wired via 4 resistors to the ADC input.

The board runs with a 4.0 MHz crystal that is different from the reference design. 
That results in a non-standard baudrate of 7680 Baud that is needed to program 
flash memory in monitor mode.