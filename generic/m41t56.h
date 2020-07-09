/*
 * m41t56.h
 *
 *  Created on: 21.06.2020
 *      Author: pantec
 *
 *      routines for the i2c-CMOS-clock M41T56
 *
 *
 */

#ifndef M41T56_H_
#define M41T56_H_


#ifdef GENERIC_CODE
	#include "i2c.h"
	// must define
	//		i2c_start()
	//		i2c_restart()
	//		i2c_tx_byte(addr)
	//		uint8_t i2c_rx_byte()
	//		acknak(bool)
	//		i2c_stop();
#else
	#include "i2c_hc908.h"
#endif

#define		M41T56_ADDR				0x68

#define		M41T56_WRITE_I2C		((M41T56_ADDR << 1) & 0xFF)
#define		M41T56_READ_I2C			(M41T56_WRITE_I2C | 0x01)


uint8_t m41t56_read_wa(uint8_t addr, uint8_t len)
{
	uint8_t r = 0;
	uint8_t i;
	//send start again
	i2c_start();

	//send address without R bit
	r = i2c_tx_byte_OK0( M41T56_WRITE_I2C );
	if (r != 0) {
		// not acked
		r |= 0x10;
		goto m41t56_read_wa_exit;
	}

	//send first register address
	r = i2c_tx_byte_OK0( addr & 0x3F );
	if (r != 0) {
		// not acked
		r |= 0x20;
		goto m41t56_read_wa_exit;
	}

	i2c_restart();

	r = i2c_tx_byte_OK0( M41T56_READ_I2C );
	if (r != 0) {
		// not acked
		r |= 0x30;
		goto m41t56_read_wa_exit;
	}

	for (i=0; i<len; i++) {
		if (i >= FIXED_I2C_DATA_LEN) {
			break;
		}
		i2c_data_buf[i] = i2c_rx_byte_noAckCheck();
		// always ACK except for the last
		r = acknak(i<len-1 ? 1 : 0);
		if (r != 0) {
			// timeout from clock stretch
			r |= 0x40;
			goto m41t56_read_wa_exit;
		}
	}

m41t56_read_wa_exit:

	//transmit stop
	i2c_stop();

	//return if slave NACKed
	return r;
}




uint8_t m41t56_write(uint8_t addr, uint8_t len)
{
	uint8_t

	r = 0;
	uint8_t i;

	//send start
	i2c_start();

	//send address without R bit
	r = i2c_tx_byte_OK0( M41T56_WRITE_I2C );
	if (r != 0) {
		// not acked
		r |= 0x10;
		goto m41t56_write_exit;
	}

	//send first register address
	r = i2c_tx_byte_OK0( addr & 0x3F );
	if (r != 0) {
		// not acked
		r |= 0x20;
		goto m41t56_write_exit;
	}

	for (i=0; i<len; i++) {
		if (i >= FIXED_I2C_DATA_LEN) {
			break;
		}

		r = i2c_tx_byte_OK0( i2c_data_buf[i] );
		if (r != 0) {
			// not acked
			r |= 0x08;
			r <<= 4;
			r |= i;
			goto m41t56_write_exit;
		}

	}

m41t56_write_exit:

	//transmit stop
	i2c_stop();

	//return if slave NACKed
	return r;
}

#if 0
// return error code or 0 if OK
uint8_t m41t56_read()
{
	uint8_t i;
	uint8_t r = 0;

	//send start again
	i2c_start();

	r = i2c_tx_byte_OK0( M41T56_READ_I2C );
	if (r != 0) {
		// not acked
		r |= 0x10;
		goto m41t56_read_exit;
	}

	for (i=0; i<FIXED_I2C_DATA_LEN; i++) {
		i2c_data_buf[i] = i2c_rx_byte_noAckCheck();
		// ALWAYS ACK
		r = acknak(1);
		if (r != 0) {
			// timeout from clock stretch
			r |= 0x20;
			goto m41t56_read_exit;
		}
	}

m41t56_read_exit:

	//transmit stop
	i2c_stop();

	return r;
}

#endif


uint16_t BCDchar(uint8_t h)
{
	h += 48;
	if (h > 57) {
		h += 7;
	}
	return h;
}

uint8_t m41t56_getClockByte(uint8_t i)
{
	switch(i) {
		case 0: return '2';
		case 1: return '0';
		case 2: return BCDchar(i2c_data_buf[6] >> 4);
		case 3: return BCDchar(i2c_data_buf[6] & 0x0F);
		case 4: return '/';
		case 5: return BCDchar(i2c_data_buf[5] >> 4);
		case 6: return BCDchar(i2c_data_buf[5] & 0x0F);
		case 7: return '/';
		case 8: return BCDchar(i2c_data_buf[4] >> 4);
		case 9: return BCDchar(i2c_data_buf[4] & 0x0F);
		case 10: return ' ';
		case 11: return BCDchar(i2c_data_buf[2] >> 4);
		case 12: return BCDchar(i2c_data_buf[2] & 0x0F);
		case 13: return ':';
		case 14: return BCDchar(i2c_data_buf[1] >> 4);
		case 15: return BCDchar(i2c_data_buf[1] & 0x0F);
		case 16: return ':';
		case 17: return BCDchar(i2c_data_buf[0] >> 4);
		case 18: return BCDchar(i2c_data_buf[0] & 0x0F);
		default: return 0;
	}
//	return 0;
}

/*
uint8_t m41t56_read_clock(uint8_t *buf)
{
	uint8_t r = m41t56_read_wa(0, 8);
	if (r != 0) {
		buf[0] = 0;
		return r;
	}
	buf[0] = '2';

	buf[19] = 0;
	return 0;
}
*/

inline void m41t56_init()
{
#ifndef WITH_MULTIPLE_I2C_CLIENTS
    // I2C to Clockchip
    i2c_init();
#endif
}


#endif /* M41T56_H_ */
