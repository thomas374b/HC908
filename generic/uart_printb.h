/*
 * uart_printb.h
 *
 *  Created on: 20.06.2020
 *      Author: pantec
 */

#ifndef UART_PRINTB_H_
#define UART_PRINTB_H_


void uart_printb_func()
{
	uint8_t z  = ' ';
	uint8_t i;

//	dumb_uart_putc('0');
//	dumb_uart_putc('b');

	for (i=0x80; i>0; i >>= 1) {
		if ((uart_print_data & i) > 0) {
			uart_putc('1');
		} else {
			uart_putc('_');
		}
	}
	uart_putc('\t');
}
#define	uart_printb(x)	{ uart_print_data = x; uart_printb_func(); }


#endif /* UART_PRINTB_H_ */
