/*
 * globalVars.h
 *
 *  Created on: 23.07.2020
 *      Author: pantec
 */


#include	"myTypes.h"
#ifdef WITH_TIMER_SYSCLOCK
#include	"timer_hc908.h"
#endif

volatile	average_parms_t		adc_data;
#ifdef WITH_TIMER_SYSCLOCK
volatile	timer_data_t 		timer_data;
#endif

#define		GLOBAL_VARS_START	((uint8_t)&adc_data)
