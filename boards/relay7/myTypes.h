/*
 * myTypes.h
 *
 *  Created on: 13.06.2020
 *      Author: pantec
 */

#ifndef MYTYPES_H_
#define MYTYPES_H_


#include "types_hc908.h"

typedef struct {
	uint8_t raw;
	int16_t sum;
	int16_t av;
	int16_t gradient;
} average8_parms_t;

extern average8_parms_t *byte32avg_data;
extern void byte32Average_func();

#define byte32Average(x)		{ byte32avg_data = x; byte32Average_func(); }



#endif /* MYTYPES_H_ */
