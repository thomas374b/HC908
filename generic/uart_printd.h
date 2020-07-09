/*
 * uart_printd.h
 *
 *  Created on: 20.06.2020
 *      Author: pantec
 */


#include "printd_type.h"

printd_int_t 	uart_print_data;

void uart_printd_func()
{
	uint8_t z  = ' ';

#ifndef WITH_UNSIGNED_PRINT_INT
	if (uart_print_data < 0) {
		z = '-';
		uart_print_data = -uart_print_data;
	}
#endif

	printd_int_t d = 10000;

	do {
		printd_int_t m = uart_print_data / d;

		if (m > 0) {
			if (z != '0') {
				uart_putc(z);
			}
			uart_putc((m % 0xFF)+48);
			z = '0';
		} else {
			if (z == '-') {
				uart_putc(' ');
			} else {
				if (d == 1) {
					uart_putc('0');
				} else {
					uart_putc(z);
				}
			}
		}
		m *= d;
		uart_print_data -= m;

		d /= 10;
	} while (d > 0);

	uart_putc('\t');

}


