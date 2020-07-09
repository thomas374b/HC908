/*
 * dumb_uart.h
 *
 *  Created on: 12.06.2020
 *      Author: pantec
 */

#ifndef DUMB_UART_H_
#define DUMB_UART_H_

//   0n := 22.4     := 44642 Baud
//   1n := 26717.5  := 37428.65


//   0 := 26.041 := 38400 Baud
//   1 := 64    := 15625 Baud
//   2 := 79.2  := 12626.26
//   3:=  94.66 := 10563 Baud
//   4 := 108.0 :=  9259.25


#define		DUMB_UART_STARTTIME		2       // 1:=40  2:=48
#define		DUMB_UART_BITTIME		1		// 1:=60/58
#define		DUMB_UART_STOPTIME		11

#define		uDelay(x)		{ d = x;	while (d-- > 0)	/*__asm__("nop\n")*/ ; }

#define		disable_ints()			__asm__("sei\n");
#define		enable_ints()			__asm__("cli\n");


#define		uDelay_DUMB_UART_STARTTIME		uDelay(DUMB_UART_STARTTIME); __asm__("nop\nnop\nnop\nnop\n");
//  #define		uDelay_DUMB_UART_STARTTIME			__asm__("nop\n");
// #define		uDelay_DUMB_UART_STARTTIME			__asm__("nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n");

// #define		uDelay_DUMB_UART_BITTIME		uDelay(DUMB_UART_BITTIME)
// #define		uDelay_DUMB_UART_BITTIME
#define		uDelay_DUMB_UART_BITTIME		__asm__("nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n");

#define		uDelay_DUMB_UART_LOWBITTIME		__asm__("nop\nnop\nnop\nnop\n");

#define		uDelay_DUMB_UART_LASTBITTIME	uDelay_DUMB_UART_LOWBITTIME; __asm__("nop\nnop\nnop\nnop\n");

#define		uDelay_DUMB_UART_STOPTIME		uDelay(DUMB_UART_STOPTIME)
// #define		uDelay_DUMB_UART_STOPTIME			__asm__("nop\n");


/*
#define		pin_high(x,y)	x |= y;
#define		pin_low(x,y)	x &= ~y;
#define		dumb_uart_out_pin			DUMB_UART_OUT_MASK
#define 	dumb_uart_pin_high()		pin_high(DUMB_UART_OUT_PORT, dumb_uart_out_pin)
#define 	dumb_uart_pin_low()			pin_low(DUMB_UART_OUT_PORT, dumb_uart_out_pin)
*/

#define 	dumb_uart_pin_high()		DUMB_UART_OUT_PIN = 1;
#define 	dumb_uart_pin_low()			DUMB_UART_OUT_PIN = 0;

#endif /* DUMB_UART_H_ */
