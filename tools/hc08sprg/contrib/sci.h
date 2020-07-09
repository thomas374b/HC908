
/******************************************************************************* 
 * 
 * (c) Freescale Semiconductor, 2004, All rights reserved  
 *  
 *******************************************************************************  
 *  
 * $File Name        : sci.h$ 
 * $Date             : Mar-14-2008$ 
 * $Version          : 6.0.11.0$  
 * $Last Modified By : r30323$ 
 *  
 * Description       : Bootloader Software SCI API wrapper 
 *  
 ******************************************************************************/  
 
 
 
/* 
 
The most of HC08/S08 MCUs can be programmed with so-called Bootloader application. 
For details look at AN2295 Application Note on Freescale Website. 
 
Since some HC08/S08s has no hardware SCI available, the Bootloader emulates it using software. 
User application now can re-use this software SCI algorithms to communicate over the 
same pins. 
 
The API consists of one table. The table *address* (pointer) is located 
at unused interrupt vector location that is specific for each SCI-less HC08/S08. 
 
SCIAPI points to --> +-------------------------------+ 
                     | SCI init routine address      | 
                     +-------------------------------+ 
                     | SCI RX routine address        | 
                     +-------------------------------+ 
                     | SCI TX routine address        | 
                     +-------------------------------+ 
                     | ONEBIT (run-time) address     | 
                     +-------------------------------+ 
 
The table contains four entries: 
 
* SCI init routine address         
  - this routine must be called before any SCI routine usage. 
  - it will initialize DDR for TX pin. 
  - it will initialize DDR & pull-up (if applicable) for RX pin. 
  - it will also initialize timing values in RAM for SCI routines for 9600Bd rate 
 
* SCI RX routine address 
  - the receive routine, the value is returned in register A 
 
* SCI TX routine address 
  - the transmit routine, the value to send expected in register A 
 
* ONEBIT (run-time) address 
  - this variable contains the value that is proportional to the datarate. Upon exit 
    from SCI init routine, it is filled with value for 9600Bd. If you wish to use e.g. 
    half data rate, read this value and mutliply it by two (see example below). 
 
There are just few restrictions: 
 
* User application may NOT use the three RAM locations specific for each HC08 (see below). 
  Just tweak .prm file for the linker won't occupy these locations. 
   
* Any interrupt source may corrupt the SCI communication so it's strongly adviced 
  to disable interrupts during SCI communications. 
   
* The SCI routines will use the timer. Only modulo counter (TMODH:TMODL) and  
  Timer Status & Control (TSC) registers are modified. 
  the timer will be stopped and TOF flag cleared on exit from both SCI routines. 
 
 
!NOTE!NOTE!NOTE!NOTE!NOTE!NOTE!NOTE!NOTE!NOTE!NOTE!NOTE!NOTE!NOTE!NOTE!NOTE!NOTE!NOTE!NOTE!NOTE! 
  
  The SCIAPI will only work correctly, if the invocation of SCI read/write routines happens 
  under the same conditions as occurs during bootloading. Mainly the clocking options are 
  to be considered.  
  QT/QY & LB8 bootloader runs using internal oscillator (12.8MHz option in case of QT/QY5/8) 
  so the SCI routines will communicate only using the same clocking option. The OSCTRIM  
  register is copied down from factory preset location ($FFC0) so it should be set the same. 
  The JK/JL version needs to be configured for each specific clock source and the SCI 
  routines will expect the same configuration too. 
 
!NOTE!NOTE!NOTE!NOTE!NOTE!NOTE!NOTE!NOTE!NOTE!NOTE!NOTE!NOTE!NOTE!NOTE!NOTE!NOTE!NOTE!NOTE!NOTE! 
 
 
SCIAPI is stored at following addreses: 
 
MC68HC908QT/QY 1/2/4  0xFFF8:0xFFF9 
MC68HC908JK/JL 1/3    0xFFF8:0xFFF9 
MC68HC908LB8          0xFFF8:0xFFF9 
MC9S08QD2/4           0xFFF8:0xFFF9 
 
Reserved RAM locations are: 
 
MC68HC908QT/QY 1/2/4  0x0080:0x0082 - total 3 bytes 
MC68HC908JK/JL 1/3    0x0080:0x0082 - total 3 bytes 
MC68HC908LB8          0x0080:0x0082 - total 3 bytes 
MC9S08QD2/4           0x0060:0x0062 - total 3 bytes 
 
 
thus your application MUST NOT use these memory locations. These will be overwritten 
during SCIAPI calls. 
 
You have to modify your application PRM file as following: 
 
original (QD4 example): 
  Z_RAM  = READ_WRITE 0x0060 TO 0x00AF;  
 
modified (QD4 example): 
  Z_RAM  = READ_WRITE 0x0063 TO 0x00AF;  
 
 
 
API usage - is simple :) 
------------------------ 
 
#include "sci.h"            // just include *THIS* file ;) 
 
unsigned char chr; 
unsigned char scispeed; 
 
void main(void) 
{ 
    /-* MCU INIT HERE *-/ 
 
    // oscillator needs to be initialized in the same configuration 
    // as the bootloader is (prescalers, trim registers, etc. 
    // otherwise SCIAPI timing will be wrong     
     
    /-* MCU INIT ENDS HERE *-/ 
     
    SCIAPIInit();           // initialize SCI API 
     
    scispeed = SCIAPISpeed; // not needed at all if default speed is to be used 
     
    /-* more stuff *-/ 
     
    chr = 'A'; 
    SCIAPIWrite(chr);       // will send 'A' 
     
    chr = SCIAPIRead();     // will read char from SCI 
 
 
    SCIAPISpeed = 2 * scispeed;       // tweak communication speed for 4800Bd 
 
    chr = 'B'; 
    SCIAPIWrite(chr);       // will send 'B' @ 4800 datarate 
     
    chr = SCIAPIRead();     // will read char from SCI @ 4800 datarate 
 
} 
 
------------------------------------------------------------------------------------------------ 
Additional notes: 
 
The developer must add or modify the appropriate section of code (defines) that are 
platform (board) specific.   
 
It is advisable to create new target in Metrowerks IDE 
Target tab in project mcp window 
menu Project > Create Target > Clone Existing target 
 
Then select this new target and in Settings (menu Edit > {platform name} settings  
(Alt+F7)) window, select 'Assembler for HC08' and change setting -DPLATFORM=1  
e.g. to -DPLATFORM=3 if your newly defined platform has number 3.  
Then select 'Linker for HC08' and change target name in Application Filename. 
 
Then modify also following setings in asm file: 
 
* memory available (e.g. 1kB, 4kB or 8kB for QT/QY) 
* which pins are used for communication and which polarity 
 
            IF PLATFORM = 3 
 
FLS_BEG		    EQU	    MEMSTART_QT4    ; specify YOUR memory limit! 
 
TXDPORT         EQU     PTA           ; <<< 
TXDPIN          EQU     3             ; <<< 
SCITXINV        EQU     INV           ; (1 if SCI TX is inverted (no drivers)) 
SCIRXINV        EQU     SCITXINV      ; (1 if SCI RX is inverted (no drivers)) 
 
RXDPORT         EQU     TXDPORT       ; <<<SINGLE WIRE 
RXDPIN          EQU     TXDPIN        ; <<<SINGLE WIRE 
     
RXDPUEN         EQU     0             ; use pull-up feature (will be done different way) 
 
            IF RXDPUEN = 1 
RXDPUE          EQU     RXDPORT+$0B   ; define pull-up enable port 
            ENDIF 
 
            ENDIF 
 
If there are RS232<>TTL level shifters on board, SCITXINV and SCIRXINV must be 0 (NOINV). 
Then log. "1" is 5V, log. "0" is 0V (this is usual setting, RS232 idle state = high, log. "1"). 
 
 
For minimal hardware configuration, SCITXINV and SCIRXINV can be defined as 1 (INV), 
then the voltage levels *are* inverted and very simple hardware interface can be used: 
 
Transmit pin on QT/QY can be directly connected to RXD pin of RS232  
= most modern serial cards do accept this 0/5V swing since the threshold is usually around 1V.  
 
Receive pin on QT/QY is connected via simple resistor & Zener diode combination: 
 
 
TX QT/QY                   RXD on RS232 (pin 3 on Cannon-9) 
pin ----------------------->>>--------- 
 
RX QT/QY        +-----+    TXD on RS232 (pin 2 on Cannon-9) 
pin ----+-------+ 10k +----<<<--------- 
        |       +-----+ 
      +--- 
      |/ \  Zener diode 5V or 3V (depending on QT/QY's Vcc) 
       --- 
        | 
       ---  GND            GND on RS232 (pin 5 on Cannon-9) 
 
 
This has been proved many times and works flawlessly on speeds above 9600Bd too. 
 
*/ 
 
#ifndef _BOOTLOADERSCIAPI_ 
#define _BOOTLOADERSCIAPI_ 1 
 
#define SCIAPIADDR 0xfff8 /* QT/QY/JK/JL/LB/S08QD families */ 
 
#define SCIAPI    (*((unsigned int *)(SCIAPIADDR))) 
 
#define SCIAPIInit  ((void (*)(void)) (*(((unsigned int *)SCIAPI)+0))) 
#define SCIAPIRead  ((unsigned char (*)(void)) (*(((unsigned int *)SCIAPI)+1))) 
#define SCIAPIWrite ((void (*)(unsigned char)) (*(((unsigned int *)SCIAPI)+2))) 
#define SCIAPISpeed (*(unsigned int *) (*(((unsigned int *)SCIAPI)+3))) 
 
#endif 


