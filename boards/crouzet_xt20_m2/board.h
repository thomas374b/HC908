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


//#define		V_ANALOG_REF		4936		// mV  average
#define		V_ANALOG_REF			4981		// mV		RMS
#define		R_LADDER_PRESCALER		6198		// factor * 1000

#define		SUM_TO_MVOLT(x)			((((x * V_ANALOG_REF) / 320UL) * R_LADDER_PRESCALER) / 100000UL)

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

#ifdef WITH_HC08SPRG_BOOTLOADER

#if 0		// hcs08prg BOOTLOADER RAM VARIABLES
volatile __data uint16_t __at 0x60 ADRS;
volatile __data  uint8_t __at 0x62 POM;
volatile __data  uint8_t __at 0x63 LEN;
volatile __data  uint8_t __at 0x64 STAT;
volatile __data  uint8_t __at 0x65 STSRSR;
volatile __data uint16_t __at 0x66 STACK;
volatile __data uint16_t __at 0x68 SOURCE;
#endif

volatile __data  uint8_t __at 0x60 memPattern;
volatile __data uint16_t __at 0x61 memAddr;
volatile __data  uint8_t __at 0x63 uart_puts_len;

typedef 	void (*funcPtr)(void);
#define 	uart_init 	((funcPtr)0xFCAD)

void uart_putc(uint8_t value)
{
	value;		// implicit given in register 'a' by compiler
	__asm
		jsr	0xFD11
	__endasm;
}

void memset_asm(uint8_t len/*, uint16_t memAddr*/)
{
	len;		// implicit given in register 'a' by compiler
//	memAddr;	// implicit given in register 'hx' by compiler
	__asm
		ldhx	_memAddr
nextByte$:
		mov		_memPattern,x+
		dbnza	nextByte$
	__endasm;
}

#define MEMSET(d,p,l)	{ memPattern = p; memAddr = d; memset_asm(l); }

void uart_puts(uint8_t len, uint16_t strAddr)
{
	len;		// compiler puts this to register 'a'
	strAddr;	// compiler puts this to register 'hx'
	__asm
		sta		_uart_puts_len
		jsr		0xFCEF
	__endasm;
}

//#define	UART_PUTS(l, addr)		{ uart_puts_len = l; uart_puts_asm(addr); }


#if 0
_VOLDATA _UINT8 __at 0x63 	uart_puts_len;

// #define	uart_puts(x, y)		uart_puts_len = y; uart_puts_1(x);



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

#endif

#ifdef WITH_COP_ENABLED
// inline
void COP_Reset()
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
#else
	#define	COP_Reset()
#endif



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

#ifdef WITH_TIMER_SYSCLOCK
	#include "timer_hc908.h"
#endif

#ifdef WITH_UART
	#include "uart_hc908.h"
#endif

#ifdef WITH_M41T56_CLOCKCHIP
	#include "m41t56.h"
#endif

#ifdef WITH_I2C_BITBANGING_BUS
	#include "i2c_hc908.h"
#endif

#ifdef WITH_DISPLAY
	#include "display.h"
	#include "truly_mcc315.c"
#endif

#ifdef WITH_ADC_CHANNELS
	#include "adc_hc908.h"
#endif

#ifdef WITH_DIALOGS
	#include "menu_dialogs.h"
#endif

#include "globalVars.h"
#include "globalVarSize.h"

inline
void board_init()
{
#if F_CPU > 2000000UL
	switch_to_pll_clock();
#endif

	MEMSET(GLOBAL_VARS_START, 0, sizeof(global_vars_t));

#ifdef WITH_TIMER_SYSCLOCK
	timer_init();
#endif
#ifdef WITH_MULTIPLE_I2C_CLIENTS
    i2c_init();
#endif
#ifdef WITH_M41T56_CLOCKCHIP
    m41t56_init();
#endif
#ifdef WITH_UART
	uart_init();
	SCRIE = 1;		// enable receiver interrupts, must come after uart_init()
#endif
#ifdef WITH_ADC_CHANNELS
	adc_init();
#endif
#ifdef WITH_MANUAL_PWM_OUT
    POUTCTL = 1;	// PWM pins manual output control, relay 0..5
#endif

    __asm__("cli");

#ifdef WITH_DISPLAY
    display_init();	// Timer and Interrupts must be running for this
#endif

}



#endif /* BOARD_H_ */
