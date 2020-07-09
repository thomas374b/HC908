This is an unfinished project that serves as an example how to use the components 
of the Crouzet XT20 SPS.

In the currect state it
    - shows the time of the internal nonvolatile CMOS clock on the display
    - reacts to some chars received from the serial line
    - outputs status of the input ports to the serial line
    - shows button-press and -release events on the serial line
    - outputs 3 (debugging) clock signals on PTA1, PTA2, PTA3

It should also
    - sample analog input from a choosen channel
    - switch a choosen relays via button-press
but the implementation is not finished yet.