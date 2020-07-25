/*
 * myTypes.h
 *
 *  Created on: 23.07.2020
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
	uint16_t count;
	sliding_avg_parms_t avg;
#ifdef WITH_NEW_STABILIZER_CODE
	avg_stabilizer_t stabi;
#endif
} average_parms_t;



#endif /* MYTYPES_H_ */
