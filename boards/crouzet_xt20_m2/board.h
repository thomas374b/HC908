/*
 * board.h
 *
 *  Created on: 19.06.2020
 *      Author: pantec
 */

#ifndef BOARD_H_
#define BOARD_H_


#include <hc08/mc68hc908mr32.h>
#include "myTypes.h"


// J1: ISP
//	J1_1	Gnd
//	J1_2	Vcc 5V
#define		J1_3	PTE2		// SCL
#define		J1_5	PTE3		// SDA
#define		J1_4	PTF4		// RxD
#define		J1_6	PTF5		// TxD

// J2: LCD
//	J2_1	Vcc
//	J2_2	Gnd
#define		J2_3	PTF3		// MISO
#define		J2_4	PTF2		// MOSI
//	J2_5	Reset
#define		J2_6	PTF0		// SCLK
#define		J2_7	PTD5
#define		J2_8	PTD6
#define		J2_9	PTC3
#define		J2_10	PTD4
//#define	J2_11
#define		J2_12	PTC2
//#define	J2_13
//#define	J2_14


// J3: Pfosten
//	J3_1	Vcc
//	J3_2	Gnd
#define		J3_3	PTC4
#define		J3_4	PTA3
#define		J3_5	PTF0		// SCLK
#define		J3_6	PTA4
#define		J3_7	PTA2
#define		J3_8	PTD4
#define		J3_9	PTA1
//	J3_10	?
#define		J3_11	PTF3		// MISO
//	J3_12	?
#define		J3_13	PTF2		// MOSI
//	J3_14	24V !!!!!!

// J4: ABCDEF
//	J4_A	?
//	J4_B	?
//	J4_C	?
//	J4_D	?
//	J4_E	?
//	J4_F	?

// J5: Side-Expansion
//	J5_1	Vcc
//	J5_2	Gnd
//	J5_3	?
//	J5_4	?
#define		J5_5	PTF0		// SCLK
//	J5_6	?
#define		J5_7	PTD4
//	J5_8	?
//	J5_9	?
//	J5_10	?
#define		J5_11	PTA5
#define		J5_12	PTF3		// MISO
#define		J5_13	PTA6
#define		J5_14	PTF2		// MOSI
#define		J5_15	PTA7
//	J5_16	24V !!!!!!

// J6: Input
#define		J6_1		PTE5
//	J6_2	?
//	J6_3	?
//	J6_4	?
#define		J6_5		PTB7
#define		J6_6		PTB6
#define		J6_7		PTB5
#define		J6_8		PTB4
#define		J6_9		PTB3
#define		J6_10		PTB2
#define		J6_11		PTB1
#define		J6_12		PTB0


#define		I2C_SCLPIN_CHAR		E
#define		I2C_SCLPIN_NUM		2			//  TCH1B

#define		I2C_SDAPIN_CHAR		E
#define		I2C_SDAPIN_NUM		3			// TCLKA

#define		SPI_LCD_SS_CHAR		C
#define		SPI_LCD_SS_NUM		2		//

#define		I2C_DTATBUF_LEN				8
#define 	I2C_CLIENT_ADDR				M41T56_ADDR
#define		FIXED_I2C_DATA_LEN			I2C_DTATBUF_LEN









inline void switch_to_pll_clock()
{
	__asm
                clr     *_PCTL
                bset	#5,*_PCTL                  ; switch PLL on
                bset	#7,*_PBWC                  ; automatic bandwidth
PLL_lock_wait:
                brclr	#6, *_PBWC,PLL_lock_wait
                bset	#4,*_PCTL                  ; select PLL clock as system
    __endasm;
}

typedef 	void (*funcPtr)(void);
#define 	uart_init 	((funcPtr)0xFCAD)

void uart_putc(uint8_t value)
{
	value;
	__asm
;		lda value
		jsr	0xFD11
	__endasm;
}

#if 0
_VOLDATA _UINT8 __at 0x63 	uart_puts_len;

// #define	uart_puts(x, y)		uart_puts_len = y; uart_puts_1(x);

void uart_puts(uint8_t len, uint16_t strAddr)
{
	len;
	strAddr;
	uart_puts_len = len;
	__asm
		ldx		3,s
		jsr		0xFD11
	__endasm;
}


uint8_t uart_getc()
{
	uint8_t result;
	__asm
		jsr		0xFD0B
		sta		1,s
	__endasm;
	return result;
}
#endif

// inline
void COP_reset()
{
	__asm
		brset	#0,*_CONFIG1,_no_cop_reset
		psha
		lda		#255
		sta		_COPCTL
		pula
_no_cop_reset:
	__endasm;
}


void BusyLoopCopy()
{
	__asm
		; cycles = (((A-3)*3 +9)*X) + 22
_MoniRomDelayLoop:					   ; [5] ; jsr
			decA                       ; [1] J     $fd21   4a
000001$:
			pshA                       ; [2]     $fd22   87
			decA                       ; [1] J     $fd23   4a
			decA                       ; [1] J     $fd24   4a
000002$:
			dbnzA    000002$           ; [3] K.    $fd25   4b fe
			pulA                       ; [2] .     $fd27   86
			dbnzX    000001$           ; [3] ..    $fd28   5b f8
			rts						   ; [4]
	__endasm;
}


// handcrafted delay for 7.3725 MHz waits 148 cycles ~20µs
void uDelay20()
{
	__asm
		psha
		pshx
		lda		#42
		ldx		#1
		jsr		_MoniRomDelayLoop
		pulx
		pula
	__endasm;
}


// handcrafted delay for 7.3725 MHz waits 1108 cycles ~150µs
void uDelay150()
{
	__asm
		psha
		pshx
		lda		#181
		ldx		#2
		jsr		_MoniRomDelayLoop
		pulx
		pula
	__endasm;
}

// handcrafted delay for 7.3725 MHz waits 14782 cycles ~2ms
void uDelay2000()
{
	__asm
		psha
		pshx
		lda		#246
		ldx		#20
		jsr		_MoniRomDelayLoop
		pulx
		pula
	__endasm;
}

// handcrafted delay for 7.3725 MHz waits 33286 cycles ~4.5ms
void uDelay4500()
{
	__asm
		psha
		pshx
		lda		#252
		ldx		#44
		jsr		_MoniRomDelayLoop
		pulx
		pula
	__endasm;
}

/*
SciInit                         .EQU    0xFCAD
ReadSci                         .EQU    0xFD0B
WriteSci                        .EQU    0xFD11
WriteSciLoop            .EQU    0xFCEF           ; Start address in HX, length in LEN
WriteSciLen                     .EQU    0x0063
*/


#include "timer_hc908.h"
// #include "adc_hc908.h"
#include "i2c_hc908.h"
#include "m41t56.h"

#include "truly_mcc315.c"



inline
void board_init()
{
    POUTCTL = 1;	// PWM pins manual output control, relay 0..5

#ifdef WITH_MULTIPLE_I2C_CLIENTS
    i2c_init();
#endif
#ifdef WITH_I2C_CLOCK
    m41t56_init();
#endif
}



#endif /* BOARD_H_ */
