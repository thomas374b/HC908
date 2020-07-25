/*
 * myFeatures.h
 *
 *  Created on: 23.07.2020
 *      Author: pantec
 */

#ifndef MYFEATURES_H_
#define MYFEATURES_H_


#define		WITH_ADC_CHANNELS			1
#define		WITH_UART					1

#define		WITH_INDIVIDUAL_GRADIENT	1
#define		WITH_AVG_ACCELERATION		1
// #define		WITH_NEW_STABILIZER_CODE	1
#define		WITH_LEAVE_EVENT			1
// #define		WITH_AVG_SHIFTING			1		// -143
#define		FIXED_AVG_SHIFT				3			// 32..48 messungen
// #define		FIXED_AVG_SHIFT				2		// 4 messungen
// #define 	WITH_FREEZING_AVG			1

// #define		VERY_SIMPLE_TIMER			1			// -30 bytes
#define		WITH_TIMER_SYSCLOCK			1


#endif /* MYFEATURES_H_ */
