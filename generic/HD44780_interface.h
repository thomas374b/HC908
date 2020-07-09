/*
 * HD44780_interface.h
 *
 *  Created on: 21.06.2020
 *      Author: pantec
 */

#ifndef HD44780_INTERFACE_H_
#define HD44780_INTERFACE_H_

typedef enum {
	// commands
	 lcd_CLEARDISPLAY = 0x01
	,lcd_RETURNHOME = 0x02
	,lcd_ENTRYMODESET = 0x04
	,lcd_DISPLAYCONTROL = 0x08
	,lcd_CURSORSHIFT = 0x10
	,lcd_FUNCTIONSET = 0x20
	,lcd_SETCGRAMADDR = 0x40
	,lcd_SETDDRAMADDR = 0x80
} hd44780_commands_e;

typedef enum {
	// flags for display entry mode
	 lcd_ENTRYRIGHT = 0x00
	,lcd_ENTRYLEFT = 0x02
	,lcd_ENTRYSHIFTINCREMENT = 0x01
	,lcd_ENTRYSHIFTDECREMENT = 0x00
} hd44780_entry_flags_e;

typedef enum {
	// flags for display on/off control
	 lcd_DISPLAYON = 0x04
	,lcd_DISPLAYOFF = 0x00
	,lcd_CURSORON = 0x02
	,lcd_CURSOROFF = 0x00
	,lcd_BLINKON = 0x01
	,lcd_BLINKOFF = 0x00
} hd44780_onoff_flags_e;

typedef enum {
	// flags for display/cursor shift
	 lcd_DISPLAYMOVE = 0x08
	,lcd_CURSORMOVE = 0x00
	,lcd_MOVERIGHT = 0x04
	,lcd_MOVELEFT = 0x00
} hd44780_cursor_flags_e;

typedef enum {
	// flags for function set
	 lcd_8BITMODE = 0x10
	,lcd_4BITMODE = 0x00
	,lcd_2LINE = 0x08
	,lcd_1LINE = 0x00
	,lcd_5x10DOTS = 0x04
	,lcd_5x8DOTS = 0x00
	,lcd_Inverse = 0x01
	,lcd_Extra = 0x02
} hd44780_function_flags_e;




#endif /* HD44780_INTERFACE_H_ */
