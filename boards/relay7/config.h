/*
 * config.h
 *
 *  Created on: 10.06.2020
 *      Author: pantec
 */

#ifndef CONFIG_H_
#define CONFIG_H_


#include "myFeatures.h"

#define		OCR1A_MAX_VAL			32768
#define		TIMER_WRAP				2090		// in ms

	// 8.125 ms resolution per count, 5:55 min range till wrap

// when running on usb08 power supply in half-speed monitor-mode
// F_CPU		1020661
#define		F_CPU					1003422

#define		FORCE_INLINE			inline



// #define 		WITH_UDELAY			1


#endif /* CONFIG_H_ */
