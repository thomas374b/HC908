/*
 * peripherie.h
 *
 *  Created on: 10.06.2020
 *      Author: pantec
 */

#ifndef PERIPHERIE_H_
#define PERIPHERIE_H_

#include <hc08/mc68hc908jkjl.h>



#define		J2_P1		PTD5			// TCH1
#define		J2_P2		PTD4			// TCH0
//;J2_P3		VDD
#define		J2_P4		EXT_IRQ		// pull down to IRQ
//;J2_P5		N_C_
#define		J2_P6		PTB2		// bootloader pull-down
#define		J2_P7		PTD2				// ADC9
#define		J2_P8		PTB1		// bootloader pull-up, debug (send) pin
// #define		J2_P9		PTD3			// ADC8
#define		J2_P10		PTB0		// boot loader data-IO

#define		DEFAULT_ADC_CHANNEL		8

#define		RELAY0		PTB3
#define		RELAY1		PTB4
#define		RELAY2		PTD6
#define		RELAY3		PTD7
#define		RELAY4		PTB5
#define		RELAY5		PTB6
#define		RELAY6		PTB7

// D5 output compare/pwm
#define		CLOCKPIN				J2_P1
#define		CLOCKPIN_DDR 			DDRD
#define		CLOCKPIN_MASK			(1 << 5)

// D4 timer 0 input capture
#define		IRLEDPIN				J2_P2
#define		IRLEDPIN_DDR 			DDRD
#define		IRLEDPIN_MASK			(1 << 4)

// B2 könnte später extra LED werden J2_P6

// D2/ADC9
#define		LEDPIN					J2_P7
#define		LEDPIN_DDR 				DDRD
#define		LEDPIN_MASK				(1 << 2)

#define		LED_on()				{ LEDPIN = 1; }
#define		LED_off()				{ LEDPIN = 0; }

#define		DUMB_UART_OUT_PIN		J2_P8
#define		DUMB_UART_OUT_MASK		(1 << 1)
#define		DUMB_UART_OUT_PORT		PORTB
#define		DUMB_UART_OUT_DDR		DDRB





#endif /* PERIPHERIE_H_ */
