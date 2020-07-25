/*
 * uart_hc908.h
 *
 *  Created on: 20.06.2020
 *      Author: pantec
 */

#ifndef UART_HC908_H_
#define UART_HC908_H_

#include "myFeatures.h"
#include "config.h"
#include "types_hc908.h"

#ifdef WITH_RECEIVING


#ifndef REC_BUF_LEN
	#define		REC_BUF_LEN		64
#endif


	typedef enum {
		uartEmpty = 0,
		uartDataOverrun =  (1 << 3),		// 0x08
		uartNoiseError  =  (1 << 2),		// 0x04
		uartFramingError = (1 << 1),		// 0x02
		uartParityError =  (1 << 0),		// 0x01
		uartBytesAvailable = 0x10,
		uartLineComplete = 0x20,
		uartFifoOverrun = 0x40,

		uartRecErrorMask =  0x0F,
	} uart_receiver_status_e;


	typedef struct {
		uint8_t udr;
		uint8_t ucsra;

		uint8_t statusMask;
		uint8_t wPtr;	//
		uint8_t rPtr;
		uint8_t rBuf[REC_BUF_LEN];
	} simple_uart_receiver_t;


//	extern char *uartCopyRecLine(char *buf, uint8_t *len);
	#define uart_bytesAvailable		(uart_receiver.statusMask & uartBytesAvailable)
	#define uart_receivedLineFeed	(uart_receiver.statusMask & uartLineComplete)

extern volatile simple_uart_receiver_t uart_receiver;


/*
uartParityError =  (1 << PE),		// 0x04
uartDataOverrun = (1 << DOR),		// 0x08
uartFramingError = (1 << FE),		// 0x10
uartFifoOverrun = 0x20,
uartRecErrorMask =  (1 << PE) | (1 << DOR) | (1 << FE),  // 0x1C
*/

#define HAVE_SCI_RECV_ISR		1
void sci_receive_ISR(void) __interrupt(_isrNo_SCIreceive)
{
	uart_receiver.ucsra = SCS1;

//	if (!(uart_receiver.ucsra & (1 << RXC))) {
//		return;
//	}

	uart_receiver.udr = SCDR;

	uint8_t tmp = (uart_receiver.ucsra & uartRecErrorMask);
	if (tmp > 0) {
		uart_receiver.statusMask |= tmp;
		return;
	}

#ifdef WITH_IGNORE_XON_XOFF
	if ((uart_receiver.udr == 0x13) || (uart_receiver.udr == 0x11)) {
		return;
	}
#endif

	uart_receiver.rBuf[uart_receiver.wPtr] = uart_receiver.udr;
	uart_receiver.wPtr++;
	uart_receiver.wPtr &= (REC_BUF_LEN-1);

	uart_receiver.statusMask |= uartBytesAvailable;

	if (uart_receiver.udr == '\n') {
		uart_receiver.statusMask |= uartLineComplete;
	}
	if (uart_receiver.wPtr == uart_receiver.rPtr) {
		uart_receiver.statusMask |= uartFifoOverrun;
#ifdef DISABLE_UART_AT_OVERRUN
		_UCSRB &=  ~(1 << RXCIE);
#endif
	}
}

inline
boolean_t uartCopyRecLine(uint8_t *buf, uint8_t *len)
{
	if (uart_receiver.wPtr == uart_receiver.rPtr) {
		if (!(uart_receiver.statusMask & uartFifoOverrun)) {
			return false;
		}
	}
	if (!(uart_receiver.statusMask & uartLineComplete)) {
		return false;
	}

	while (uart_receiver.rPtr != uart_receiver.wPtr) {
		buf[*len] = uart_receiver.rBuf[uart_receiver.rPtr];
		(*len)++;
		uart_receiver.rPtr++;
		uart_receiver.rPtr &=  (REC_BUF_LEN-1);
	}
	buf[*len] = '\0';

	uart_receiver.statusMask &= ~(uartBytesAvailable | uartLineComplete);

	return true;
}

inline
uint8_t nextUartByte()
{
	uint8_t result = uart_receiver.rBuf[uart_receiver.rPtr];
	uart_receiver.rPtr++;
	uart_receiver.rPtr &=  (REC_BUF_LEN-1);

	if (uart_receiver.rPtr == uart_receiver.wPtr) {
		uart_receiver.statusMask &= ~uartBytesAvailable;
	}
	return result;
}


#endif

#endif /* UART_HC908_H_ */
