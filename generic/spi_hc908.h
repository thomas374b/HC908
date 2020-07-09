/*
 * spi_hc908.h
 *
 *  Created on: 21.06.2020
 *      Author: pantec
 */

#ifndef SPI_HC908_H_
#define SPI_HC908_H_

#include "myFeatures.h"
#include "config.h"
#include "myTypes.h"


#ifndef SPI_RECBUF_LEN
	#define		SPI_RECBUF_LEN		8
#endif

#ifndef SPI_CPOL_MODE
	#define		SPI_CPOL_MODE		0
#endif

#ifndef	SPI_CPHA_MODE
	#define		SPI_CPHA_MODE		0
#endif

typedef enum {
		spiEmpty = 0,
		spiFifoOverrun = 1,
		spiOverflow = _BV(5),
		spiModeFault = _BV(4),
		spiBytesAvailable = _BV(7),
} spi_receiver_status_e;

#define		spiRecErrorMask			(spiModeFault | spiOverflow)
#define 	spi_bytesAvailable		(spi_receiver.statusReg & spiBytesAvailable)


typedef struct {
	uint8_t dataReg;
	uint8_t statusReg;
	uint8_t wPtr;	//
	uint8_t rPtr;
	uint8_t rBuf[SPI_RECBUF_LEN];
} spi_receiver_data_t;

extern volatile spi_receiver_data_t spi_receiver;


#define HAVE_SPI_RECV_ISR		1
void spi_receive_ISR(void) __interrupt(_isrNo_SPIreceive)
{
	uint8_t tmp = SPSCR;
	spi_receiver.dataReg = SPDR;
	SPRF = 0;


	if ((tmp & spiRecErrorMask) > 0) {
		spi_receiver.statusReg |= (tmp & spiRecErrorMask);
		return;
	}
	if ((tmp & spiBytesAvailable) > 0) {
		spi_receiver.statusReg |= spiBytesAvailable;
	}

	spi_receiver.rBuf[spi_receiver.wPtr] = spi_receiver.dataReg;
	spi_receiver.wPtr++;
	spi_receiver.wPtr &= (SPI_RECBUF_LEN-1);

	if (spi_receiver.wPtr == spi_receiver.rPtr) {
		spi_receiver.statusReg |= spiFifoOverrun;
#ifdef DISABLE_SPI_AT_OVERRUN
		_UCSRB &=  ~(1 << RXCIE);
#endif
	}
}


inline
uint8_t nextSpiByte()
{
	uint8_t result = spi_receiver.rBuf[spi_receiver.rPtr];
	spi_receiver.rPtr++;
	spi_receiver.rPtr &=  (SPI_RECBUF_LEN-1);

	if (spi_receiver.rPtr == spi_receiver.wPtr) {
		spi_receiver.statusReg &= ~spiBytesAvailable;
	}
	return result;
}



inline void spi_init()
{
	SPMSTR = 1;

	CPOL = SPI_CPOL_MODE;
	CPHA = SPI_CPHA_MODE;

// CPHA == 0  =>> toggle SS manually each byte
// CPHA == 1  => only begin end of transaction

	MODFEN = 1;	// PTF1 is available as general IO

	// divider for SPI clock derived from system clock  {4,16,64,256}
#ifdef WITH_DEBUG
	// try slowest clock first for debugging
//	SPR1 = 1;	SPR0 = 1;   // 28.7988 kHz @ 7.3725 MHz sysclk
#else
//	SPR1 = 1;	SPR0 = 0;	// 115.195 kHz @ 7.3725 MHz sysclk
//	SPR1 = 0;	SPR0 = 1;	// 460.781 kHz @ 7.3725 MHz sysclk
	SPR1 = 0;	SPR0 = 0;	// 1.843125 MHz @ 7.3725 MHz sysclk
#endif

	SPE = 1;	// enable SPI interface
	SPRIE = 1; 	// enable receiver interrupt
}


void spi_write(uint8_t data)
{
#ifdef WITH_STACK_TEST
	testStack();
#endif
	while (SPTE == 0) {};
	SPDR = data;
}


#endif /* SPI_HC908_H_ */
