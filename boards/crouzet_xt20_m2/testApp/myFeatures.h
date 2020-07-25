/*
 * myFeatures.h
 *
 *  Created on: 13.06.2020
 *      Author: pantec
 */

#ifndef MYFEATURES_H_
#define MYFEATURES_H_


// quickly rule out parts of code for debugging
#define		WITH_KEYBOARD					1
#define		WITH_M41T56_CLOCKCHIP			1
#define		WITH_ADC_CHANNELS				1

// #define			WITH_STACK_TEST

#define		WITH_BITREV_LOOKUP_TABLE		1


// timer
#define		WITH_TIMER_SYSCLOCK				1
#define		VERY_SIMPLE_TIMER				1
#define		WITH_TDELAY						1
	// have a more precise busy loop with break for timeouts on waiting for the HW
//	#define		WITH_SLOTTED_GETTIME		1
	// only one single call to getTime() during one loop
// #define		WITH_GETTIME_FUNC			1
// #define		WITH_PRECISE_TIMING			1	// avoid this, not enough tested


// uart
#define		WITH_UART						1
#define		WITH_RECEIVING					1



#define		WITH_UNSIGNED_PRINT_INT			1


#define		WITH_DISPLAY					1

// keyMatrix.h
//#define		WITH_BITWISE_DOUBLE_KEY			1		// +6/+6
	// have additional virtual keys by simultaneous pressing two


// bitbanging i2C
// #undef	WITH_MULTIPLE_I2C_CLIENTS

#define		WITH_AVG_ACCELERATION			1
#define		WITH_INDIVIDUAL_GRADIENT		1
#define		WITH_NEW_STABILIZER_CODE		1
//#define		WITH_TRACE_MIN_MAX				1

#define		WITH_DIALOGS					1		// implementation not yet finished

#define		WITH_PIE_SECONDS_COUNTER		1
	// show binary clock with the pie chart

#define		WITH_MANUAL_PWM_OUT				1



#endif /* MYFEATURES_H_ */
