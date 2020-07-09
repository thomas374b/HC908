/*
 * byte8_average.c
 *
 *  Created on: 13.06.2020
 *      Author: pantec
 */

#ifndef BYTE8_AVERAGE_C_
#define BYTE8_AVERAGE_C_


#include "project.h"


average8_parms_t *byte32avg_data;

void byte32Average_func()
{
	byte32avg_data->av = byte32avg_data->sum;
	byte32avg_data->av /= 6;
	byte32avg_data->gradient = -byte32avg_data->sum;
	byte32avg_data->sum -= byte32avg_data->av;
	byte32avg_data->sum += byte32avg_data->raw;
	byte32avg_data->gradient += byte32avg_data->sum;
}



#endif /* BYTE8_AVERAGE_C_ */
