/*
 * crouzet_lcd_keys.h
 *
 *  Created on: 22.06.2020
 *      Author: pantec
 *
 *   bit definitions for truly mcc315 LCD key-matrix
 *   for a specific design/architecture
 *
 */

#ifndef CROUZET_LCD_KEYS_H_
#define CROUZET_LCD_KEYS_H_

#include 	"myTypes.h"


#define		BIT_ROW_CHAR		C
#define		BIT_COL_CHAR		D

#define		KEY_PRESS_BIT		0	// not real hardware but included into the mask
#define		KEY_RELEASE_BIT		1	// since the need to occupy one bit
#define		BIT_ROW1_NUM		2
#define		BIT_ROW0_NUM		3
#define		BIT_COL0_NUM		4
#define		BIT_COL1_NUM		5
#define		BIT_COL2_NUM		6

#define		BIT_ROW_EN_CHAR		C
#define		BIT_ROW_EN_NUM		5


#endif /* CROUZET_LCD_KEYS_H_ */
