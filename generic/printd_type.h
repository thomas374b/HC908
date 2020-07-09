/*
 * printd_type.h
 *
 *  Created on: 21.06.2020
 *      Author: pantec
 */

#ifndef PRINTD_TYPE_H_
#define PRINTD_TYPE_H_

#include "myFeatures.h"

#ifdef	WITH_UNSIGNED_PRINT_INT
	typedef 	uint16_t	printd_int_t;
#else
	typedef 	int16_t		printd_int_t;
#endif

extern printd_int_t 	uart_print_data;
extern void uart_printd_func();

#define	uart_printd(x)	{ uart_print_data = (printd_int_t)x; uart_printd_func(); }


#endif /* PRINTD_TYPE_H_ */
