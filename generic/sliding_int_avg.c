/*
 * sliding_int_avg.c
 *
 *  Created on: 08.01.2018
 *      Author: pantec
 */


#include "sliding_int_avg.h"



#ifdef WITH_AVG_ACCELERATION
	#ifdef WITH_PRECALCULATED_VALUE
		#error "WITH_PRECALCULATED_VALUE and WITH_AVG_ACCELERATION are mutually exclusive!";
	#endif
#endif

#ifdef WITH_INDIVIDUAL_GRADIENT
#else
	int16_t _last_global_gradient;
#endif

/*
void simpleAvg(int16_t *avg, int16_t val, int8_t len)
{
	int16_t grad = (val - (*avg / len));
	*avg += grad;
}
*/

#define	SIGN(x)		((x>0)?1:((x<0)?-1:0))
#define DIV2(x)		(1 << (x-1))

int16_t divide(int16_t v)
{
#ifdef WITH_AVG_SHIFTING
	return v >> FIXED_AVG_SHIFT;
#else
	// avoid rounding errors
	return (v + DIV2(FIXED_AVG_SHIFT)) / (1 << FIXED_AVG_SHIFT);
#endif
}


void intAverage(sliding_avg_parms_t *avg, avg_value_t val)
{
	if (avg->sum == 0) {
		avg->sum = AvgMultiply(val, *avg);
	}
#ifdef WITH_PRECALCULATED_VALUE
	avg->value = AvgIntValue(*avg);
#endif

	int16_t lastGrad = Gradient(*avg);

#ifdef WITH_INDIVIDUAL_GRADIENT
	avg->gradient
#else
	_last_global_gradient
#endif
		= val -
#ifdef WITH_PRECALCULATED_VALUE
				avg->value;
#else
				divide(avg->sum);
#endif

#ifdef WITH_AVG_ACCELERATION
	avg->sum += Gradient(*avg);

	int16_t accel = Gradient(*avg) - lastGrad;
#ifdef WITH_NEW_STABILIZER_CODE
	avg->aDelta = (accel - divide(avg->accelAvg));
	avg->accelAvg += avg->aDelta;
#else
	avg->accelAvg += (accel - divide(avg->accelAvg));
#endif
	avg->gradAvg  += (Gradient(*avg) - divide(avg->gradAvg));
#else
	avg->sum += Gradient(*avg);
#endif
}

#ifdef WITH_NEW_STABILIZER_CODE

uint8_t averageStabilizedEvent(sliding_avg_parms_t *avg, avg_stabilizer_t *stabi)
{
	stabi->rms += (_ABS(avg->aDelta) - divide(stabi->rms));

	int16_t vel = divide(avg->gradAvg);
	int16_t acc = divide(avg->accelAvg);

	int8_t sign = SIGN(vel) * SIGN(acc);
	int8_t result = false;

	vel = _ABS(vel);
	acc = _ABS(acc);

	if (sign <= 0) {		// value about to stabilize if acceleration crosses zero
		if (!stabi->isBraking) {
			stabi->isBraking = 1;
#ifdef WITH_FREEZING_AVG
			reset_freezing(val, stabi->freezer);
#endif
		}
		stabi->threshold = divide(stabi->rms)*2 +1;
	} else {
		if (vel > stabi->threshold) {
			if (acc > stabi->threshold) {
				if (stabi->isBraking) {
#ifdef WITH_LEAVE_EVENT
					result = true;
#endif
				}
				stabi->isBraking = false;
				stabi->isStable = false;
			}
		}
	}
#ifdef WITH_FREEZING_AVG
	add_freezing(val, stabi->freezer);
#endif
	if (stabi->isBraking) {
		if (vel < stabi->threshold) {
			if (acc < stabi->threshold) {
				if (!stabi->isStable) {
					result = true;
					stabi->isStable = true;
				}

			}
		}
	}
	return result;
}

#endif



#ifdef WITH_ACCELERATION_STRUCT
void intAccAverage(accel_avg_t *avg, int16_t val)
{
	intAverage(&avg->slider, val);
	intAverage(&avg->gradient, Gradient(avg->slider) );
}
#endif



#ifdef WITH_TRACE_MIN_MAX
void resetMinMax(sliding_avg_parms_t *avg)
{
	avg->min = 0;
	avg->max = 0;
}
#endif



