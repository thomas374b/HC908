/*
 * truly_mcc315.h
 *
 *  Created on: 21.06.2020
 *      Author: pantec
 *
 *   TRULY MCC315-A  TR-805 94V-0
 *
 *   this is sort of a hd44780 "compatible" clone with SPI transport
 *   and a built-in 2x3 key-matrix
 *
 *   one line is shared between selection of a row and SPI slave select
 *
 *   the display is in nibble (4-bit) mode where the rs- and rw- signal is
 *   embedded in a prefixed start-byte
 *
 *   the data is LSB first
 *
 *   it has some special symbols in the top row which are accessed by
 *   writing to CGRAM in extended function mode
 *
 */

#ifndef TRULY_MCC315_H_
#define TRULY_MCC315_H_

#include "HD44780_interface.h"





typedef enum {
	 lcd_Command =    0xF8
	,lcd_Write = 	  0xFA
	,lcd_ReadAddrBF = 0xFC
	,lcd_ReadRAM    = 0xFE
//	,lcd_ReadRAM 	= 0xFB		// another guess
} hd44780_write_mode_e;



typedef enum {
	symRotor0  = 0x01,					// @ cgram 0
	symRotor1  = 0x02,
	symRotor2  = 0x04,
	symRotor3  = 0x08,
	symRotor4  = 0x10,

	symRotor5  = 0x20 | 0x10,			// 0x10 @ cgram 1

	symMenu	   = 0x40 | 0x01,			// 0x01 @ cgram 2

	symWarn	   = 0x60 | 0x10,			// 0x10 @ cgram 5

	symMoni	   = 0x80 | 0x04,			// 0x04 @ cgram 7

	symKey     = 0xA0 | 0x04,			// 0x04 @ cgram 9

	symAC	   = 0xC0 | 0x10,			// 0x10 @ cgram 11

	symMASK	   = 0x1F,
	symCache   = 0xE0,
} top_row_symbols_e;


typedef enum {
	symBLINK   = 0x80,
	symON      = 0x01,
	symOFF     = 0x00,
} top_row_attr_e;


typedef struct {
	  uint8_t displaycontrol;
	  uint8_t displaymode;
//	  hd44780_write_mode_e writeMode;
	  uint8_t transferBuf[3];			// spi buffer for read/write IO
	  uint8_t symbolCache[7];			// cached bits for special symbols
} lcd_data_t;



// Client ‰ndert Daten an der fallenden Flanke  =>  CPOL muﬂ Null sein
// damit die daten sicher gelesen werden muﬂ cpha auch null sein
//   #include this definition before spi.h, important !
#define		SPI_CPOL_MODE		0
#define		SPI_CPHA_MODE		0



// you must instantiate this structure in your main program or as global
extern 		lcd_data_t 			mcc315_data;

// you must implement these functions somewhere else
extern void uDelay150();
extern void uDelay2000();
extern void uDelay4500();


// functions defined by this module
extern uint8_t mcc315lcd_transport(hd44780_write_mode_e mode);

extern void mcc315lcd_clear();
extern void mcc315lcd_home();
extern void mcc315lcd_write(uint8_t value);
extern void mcc315lcd_writeString(uint8_t *addr);
extern void mcc315lcd_setCursor(uint8_t col, uint8_t row);
extern void mcc315lcd_setSymbol(top_row_symbols_e sym, top_row_attr_e attr);


#endif /* TRULY_MCC315_H_ */
