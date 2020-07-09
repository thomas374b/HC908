/*
 * dumb_uart.c
 *
 *  Created on: 29.01.2020
 *      Author: pantec
 */


#include "dumb_uart.h"

#include "delay.h"

/*
#define	dumb_uart_out_pin			DUMB_UART_OUT_MASK
#define dumb_uart_pin_high()		pin_high(DUMB_UART_OUT_PORT, dumb_uart_out_pin)
#define dumb_uart_pin_low()			pin_low(DUMB_UART_OUT_PORT, dumb_uart_out_pin)
*/

FORCE_INLINE
void dumb_uart_init()
{
	DUMB_UART_OUT_DDR |= DUMB_UART_OUT_MASK;
}

uint8_t uart_putc_data;

//FORCE_INLINE
void dumb_uart_putc_func()
{
	uint8_t d;
	uint8_t m;

	disable_ints();

	dumb_uart_pin_low();

	uDelay_DUMB_UART_STARTTIME;

	m = 0x01;
	for (uint8_t i=0; i<8; i++) {

		if ((uart_putc_data & m) > 0) {
			dumb_uart_pin_high();
		} else {
			dumb_uart_pin_low();
			uDelay_DUMB_UART_LOWBITTIME
		}

		m <<= 1;

		uDelay_DUMB_UART_BITTIME;
	}

	uDelay_DUMB_UART_LASTBITTIME;

	dumb_uart_pin_high();

	uDelay_DUMB_UART_STOPTIME;

	enable_ints();
}

#define	dumb_uart_putc(x) 		{ uart_putc_data = x; dumb_uart_putc_func(); }


