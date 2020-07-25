/*
 * adc_hc908.h
 *
 *  Created on: 13.06.2020
 *      Author: pantec
 */

#ifndef ADC_HC908_H_
#define ADC_HC908_H_

#include "project.h"


extern volatile average_parms_t adc_data;

inline
void adc_init()
{
	ADCLK = ((_BV(7) | _BV(2)) & 0xFF); 		// right justified mode, slowest clock
//	ADIV2 = 1;			// slowest clock, F_CPU/16

	ADSCR = (1 << 6) /*| (1 << 5)*/ | DEFAULT_ADC_CHANNEL;
			//enable ADC interrupts, continuous conversion
//	ADSCR = 0;			// disable ADC interrupts
}


#define		HAVE_ADC_ISR		1
void adcIRQ(void) __interrupt(_isrNo_ADC) // __naked
{
//	ADSCR &= ~((1 << 6) | (1 << 5));	// disable continuous conversion, interrupts
	adc_data.raw = ADR;
	AIEN = 0;
/*
	__asm
		lda			*_ADRL	 // dummy read to finish the job
	__endasm;
*/
}



#endif /* ADC_HC908_H_ */
