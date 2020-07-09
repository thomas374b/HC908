/*
 * uart_printh.h
 *
 *  Created on: 20.06.2020
 *      Author: pantec
 */

#ifndef UART_PRINTH_H_
#define UART_PRINTH_H_


#include "printd_type.h"

void uart_printh_core()
{
	uint8_t h = uart_print_data & 0xF0;
	h >>= 4;

	h += 48;
	if (h > 57) {
		h += 7;
	}
	uart_putc(h);

	h = uart_print_data & 0x0F;

	h += 48;
	if (h > 57) {
		h += 7;
	}
	uart_putc(h);
}

void uart_printh_func()
{
	uart_putc('x');

	uart_printh_core();

	uart_putc('\t');
}

#define	uart_printh(x)	{ uart_print_data = x; uart_printh_func(); }

#define	uart_printhx(x)	{ uart_print_data = (x >> 8); uart_printh_core(); uart_print_data = x & 0xFF; uart_printh_core(); }



#endif /* UART_PRINTH_H_ */
