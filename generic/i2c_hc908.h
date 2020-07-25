/*
 * i2c_hc908.h
 *
 *  Created on: 20.06.2020
 *      Author: pantec
 *
 *
 *    bit-banging i2c for hc908
 *
 *    copied from linux kernel i2c-algo-bit.c and adapted
 *
 */

#ifndef I2C_HC908_H_
#define I2C_HC908_H_

#include "project.h"


#if (!defined(I2C_SDAPIN_CHAR) || !defined(I2C_SDAPIN_NUM))
	#warning "SDA pin undefined"
#endif


#if (!defined(I2C_SCLPIN_CHAR) || !defined(I2C_SCLPIN_NUM))
	#warning "SCL pin undefined"
#endif

#define		I2C_SCLPIN_DDR		DEF_DDR(I2C_SCLPIN_CHAR)
#define		I2C_SCLPIN_PORT		DEF_PORT(I2C_SCLPIN_CHAR)
#define		I2C_SCLPIN_MASK		_BV(I2C_SCLPIN_NUM)
#define		I2C_SCLPIN			DEF_PTX(I2C_SCLPIN_CHAR, I2C_SCLPIN_NUM)

#define		I2C_SDAPIN_DDR		DEF_DDR(I2C_SDAPIN_CHAR)
#define		I2C_SDAPIN_PORT		DEF_PORT(I2C_SDAPIN_CHAR)
#define		I2C_SDAPIN_MASK		_BV(I2C_SDAPIN_NUM)
#define		I2C_SDAPIN			DEF_PTX(I2C_SDAPIN_CHAR, I2C_SDAPIN_NUM)


typedef struct {
	uint8_t	recv_error;
	uint8_t buffer[I2C_DTATBUF_LEN];
} i2c_data_t;

extern 	i2c_data_t	i2c_data;


//setup bit bang I2C pins
inline void i2c_init()
{
	//set pins as inputs
	I2C_SDAPIN_DDR &= ~(I2C_SDAPIN_MASK | I2C_SCLPIN_MASK);

	//set output to low
	I2C_SDAPIN_PORT &= ~(I2C_SDAPIN_MASK | I2C_SCLPIN_MASK);
}

#if 0
#define		N_HALF_CLOCK_LOOPS		118
#if ((((50 * F_CPU) + 1500000UL)/ 3000000UL) -5) != N_HALF_CLOCK_LOOPS
	#warning "this compiler/assembler is a shitty bitch"
	#error "BIG FAT WARNING! dont put preprocessor calculations into assembly code !!!"
#endif
#endif
/**BIG FAT WARNING! dont put preprocessor calculations into assembly code !!!**/
#define		N_HALF_CLOCK_LOOPS		10		// manual adjusted after scoped
#define		N_FULL_CLOCK_LOOPS		20
/**BIG FAT WARNING! dont put preprocessor calculations into assembly code !!!**/

//wait for 1/2 of I2C clock period
inline void i2c_hc(void)
{
	//wait for 0.05ms i.e. 50µs
	// 9 ticks for jsr,rts 2 ticks loading value, 2 push, 2 pull, 15 ticks overhead
	__asm
		psha
		ldA		#N_HALF_CLOCK_LOOPS
;0001$:
		nop
;		dbnzA	0001$
		.db		#0x4B
		.db		#0xFD
		pula
	__endasm;
}


//wait for 1/2 of I2C clock period
inline void i2c_fc(void)
{
	//wait for 0.05ms i.e. 50µs
	// 9 ticks for jsr,rts 2 ticks loading value, 2 push, 2 pull, 15 ticks overhead
	__asm
		psha
		ldA		#N_FULL_CLOCK_LOOPS
;0001$:
		nop
;		dbnzA	0001$
		.db		#0x4B
		.db		#0xFD
		pula
	__endasm;
}

//wait for 1/2 clock
// i2c_hc();

inline
void pull_sda_low()
{
	// switch to output forces wired-or line low
	I2C_SDAPIN_DDR |= I2C_SDAPIN_MASK;
}

inline
void let_sda_float()
{
	// switch to input, pull-up resistor or client getting control of the wire
	I2C_SDAPIN_DDR &= ~I2C_SDAPIN_MASK;
}

inline
void pull_scl_low()
{
	// switch to output forces wired-or line low
	I2C_SCLPIN_DDR |= I2C_SCLPIN_MASK;
}

inline
void let_scl_float()
{
	// switch to input, pull-up resistor or client getting control of the wire
	I2C_SCLPIN_DDR &= ~I2C_SCLPIN_MASK;
}


uint8_t check_scl_high()
{
	let_scl_float();

	uint8_t i = 0;
	do {
		i++;
		if (I2C_SCLPIN > 0) {
			break;
		}
		if (i > 250) {	// max. clock stretching
			return 1;	// timeout
		}
		__asm
			nop
			nop
			nop
			nop
		__endasm;
	} while (1);

	return 0;
}

uint8_t check_sda_high()
{
	let_sda_float();

	uint8_t i = 0;
	do {
		i++;
		if (I2C_SDAPIN > 0) {
			break;
		}
		if (i > 250) {	// max. clock stretching
			return 1;	// timeout
		}
	} while (1);

//	//wait for 1/2 clock
//	i2c_hc();
	return 0;
}


/* --- other auxiliary functions --------------------------------------	*/


inline
void i2c_start(void)
{
	/* clock should be always high */

	pull_sda_low();

	i2c_fc();	// full clock delay

	pull_scl_low();
}

inline
void i2c_restart(void)
{
	/* assert: scl is low */
	let_sda_float();
	//wait for 1/2 clock
	i2c_hc();

	let_scl_float();
	//wait for 1/2 clock
	i2c_hc();

	pull_sda_low();
	i2c_fc();	// FULL delay

	pull_scl_low();
	//wait for 1/2 clock
	i2c_hc();
}


inline
void i2c_stop(void)
{
	/* assert: scl is low */
	pull_sda_low();
	//wait for 1/2 clock
	i2c_hc();

	let_scl_float();
	//wait for 1/2 clock
	i2c_hc();

	let_sda_float();

	i2c_fc();		// FULL clock wait
}


// send value over I2C return 0 if slave ACKed
uint8_t i2c_tx_byte_OK0(uint8_t val)
{
	uint8_t i;
	// sda should be high
//	i = check_sda_high();
//	if (i > 0) {
//		return 4;
//	}
	//wait for 1/2 clock for end of start
	i2c_hc();

	//shift out bits
	for (i=0;i<8;i++) {
		//check bit
		if	(val & 0x80) {
			let_sda_float();
		} else {
			pull_sda_low();
		}

		if (check_scl_high() == 1) {
			// timeout
			return 1;
		}
		//wait for 1/2 clock for end of start
		i2c_hc();

		val <<= 1;	//shift

		pull_scl_low();

		//wait for 1/2 clock for end of start
		i2c_hc();
	}
	let_sda_float();

	//wait for 1/2 clock for end of start
	i2c_hc();

	if (check_scl_high() == 1) {
		return 2;	// timeout waiting for ack
	}

	i = 3;	 // not acked
	if (I2C_SDAPIN == 0) {
		i =  0;	// OK
	}
	pull_scl_low();
	//wait for 1/2 clock for end of start
	i2c_hc();

	return i;
}



//read value from I2C, ack if last
uint8_t i2c_rx_byte_noAckCheck()
{
	uint8_t i;
	uint8_t val = 0;
	i2c_data.recv_error = 0;

	let_sda_float();

	//wait for 1/2 clock for end of start
	i2c_hc();

	//shift out bits
	for(i=0; i<8; i++) {
		if (check_scl_high() == 1) {
			// timeout
			i2c_data.recv_error = 1;
			return 0;
		}

		//shift value to make room
		val <<= 1;

		//sample data
		if (I2C_SDAPIN) {
			val |= 1;
		}

		pull_scl_low();
		if (i < 7) {
			i2c_fc();
		} else {
			//wait for 1/2 clock
			i2c_hc();
		}
	}
	return val;
}

// 0 := OK
inline
uint8_t acknak(uint8_t is_ack)
{
	/* assert: sda is high */
	if (is_ack > 0) {		/* send ack */
		pull_sda_low();
	}
	//wait for 1/2 clock
	i2c_hc();

	if (check_scl_high() == 1) {
		return 1;
	}
	pull_scl_low();

	//wait for 1/2 clock
	i2c_hc();

	return 0;
}



#ifdef WITH_I2C_TRANMIT_MSG
uint8_t i2c_tx(
#ifdef WITH_MULTIPLE_I2C_CLIENTS
		uint8_t addr, const uint8_t *dat
#endif
#if defined(FIXED_I2C_DATA_LEN)
#else
		, uint8_t len
#endif
		)
{
	uint8_t ack;
	uint8_t i;

	//send start
	i2c_start();

	//send address with W bit
	ack = i2c_tx_byte( (
#ifdef WITH_MULTIPLE_I2C_CLIENTS
			addr
#else
			I2C_CLIENT_ADDR
#endif
			<< 1)
			);

	//send data bytes
	for(i=0; i<
#if defined(FIXED_I2C_DATA_LEN)
			FIXED_I2C_DATA_LEN
#else
			len
#endif
			&& ack; i++) {
		//transmit next byte
		ack = i2c_tx_byte(
#ifdef WITH_MULTIPLE_I2C_CLIENTS
				dat[i]
#else
				i2c_data_buf[i]
#endif
				    );
	}
	//transmit stop
	i2c_stop();
	//return if slave NACKed
	return ack;
}
#endif

#if 0

uint8_t i2c_rx(
#ifdef WITH_MULTIPLE_I2C_CLIENTS
		uint8_t addr, uint8_t *dest
#endif
#if defined(FIXED_I2C_DATA_LEN)
#else
		, uint8_t len
#endif
		)
{
	uint8_t i;
	//send start
	i2c_start();

	//send address with R bit
	if (!i2c_tx_byte( (uint8_t) (
#ifdef WITH_MULTIPLE_I2C_CLIENTS
			addr
#else
			I2C_CLIENT_ADDR
#endif
			<< 1 ) | 1 ) ) {
		//got NACK return error
		return 0;
	}
	// read data bytes
	for (i=0;i <
#if defined(FIXED_I2C_DATA_LEN)
			FIXED_I2C_DATA_LEN
#else
			len
#endif
			;i++) {
		// receive next byte
#ifdef WITH_MULTIPLE_I2C_CLIENTS
		dest[i]
#else
		i2c_data_buf[i]
#endif
		 	 	 	 = i2c_rx_byte(i==FIXED_I2C_DATA_LEN-1);
	}
	//transmit stop
	i2c_stop();

	//return if slave NACKed
	return 1;
}

#endif




#if 0
uint8_t t41m56_write_zero()
{
	i2c_start();

	//send address with R bit
	if (! i2c_tx_byte( T41M56_WRITE_I2C )) {
		//got NACK return error
		return 0;
	}
	// write register address
	if (! i2c_tx_byte( 0 )) {
		//got NACK return error
		return 0;
	}
	// write data
	if (! i2c_tx_byte( 0 )) {
		//got NACK return error
		return 0;
	}
	//transmit stop
	i2c_stop();

	return 1;
}
#endif






#endif /* I2C_HC908_H_ */
