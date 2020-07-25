/*
 * myTypes.h
 *
 *  Created on: 13.06.2020
 *      Author: pantec
 */

#ifndef MYTYPES_H_
#define MYTYPES_H_

#include "myFeatures.h"

#include "config.h"

#include "types_hc908.h"

#include "token_pasting.h"

#include "sliding_int_avg.h"

typedef struct {
	uint16_t raw;
	uint8_t count;
	sliding_avg_parms_t avg[4];
	avg_stabilizer_t stabi[4];
} average_parms_t;


#ifdef WITH_STACK_TEST
	extern void  testStack();
#endif


#endif /* MYTYPES_H_ */
