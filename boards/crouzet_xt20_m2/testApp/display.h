/*
 * display.h
 *
 *  Created on: 30.06.2020
 *      Author: pantec
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_

#include 	"truly_mcc315.h"

#define		display_init()				mcc315lcd_init()
#define		display_clear()				{ mcc315lcd_clear(); mcc315lcd_home(); }
#define		display_home()				mcc315lcd_home()
#define		display_write(c)			mcc315lcd_write(c)
#define 	display_writeString(x)		mcc315lcd_writeString(x)
#define 	display_setCursor(c, r)		mcc315lcd_setCursor(c, r)
#define 	display_setSymbol(s, b)		mcc315lcd_setSymbol(s, b)


#endif /* DISPLAY_H_ */
