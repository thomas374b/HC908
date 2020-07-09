/*
 * config.h
 *
 *  Created on: 10.06.2020
 *      Author: pantec
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include "myFeatures.h"

//#define		OCR1A_MAX_VAL			32768UL		// save some memory by shifting
//#define		TIMER_WRAP				284

//#define		OCR1A_MAX_VAL			14422UL
//#define		TIMER_WRAP				125

//#define		OCR1A_MAX_VAL			28845UL
//#define		TIMER_WRAP				250

#define			OCR1A_MAX_VAL			0UL		// 65536		// save some memory, avoid multiplication
#define			TIMER_WRAP				568		// timer wrap in ms
	//     2.222 ms resolution per timer unit,     2:25 min range until 'now' will wrap
	// 0.568 s      resolution per wrap-count, 10:20:23 h   range until 'wrap_count' will wrap

//#define		OCR1A_MAX_VAL			57690UL
//#define		TIMER_WRAP				500


// when running without PLL feature
// #define		F_CPU			1228750
// #define		TIMER_WRAP		1710		// in ms
	// 13.333 ms resolution per count, 25:00 min range till wrap

// running with PLL
#define		F_CPU				7372500UL

// default timer for system clock
//   #define		TIMER_N				B
//   #define		CHANNEL_N			1


// #define		FORCE_INLINE		inline
#define		FORCE_INLINE

// for uart_hc908.h
#define		REC_BUF_LEN					16

// for key_matrix.h
#define		KEY_DEBOUNCE_CNT			16

// for sliding_int_avg.h
#define		AVG_STABILIZER_THRESHOLD	6
#define		AVG_STABILIZER_ZERO			2

// for acdc_hc908.h
#define		DEFAULT_ADC_CHANNEL			0


#endif /* CONFIG_H_ */
