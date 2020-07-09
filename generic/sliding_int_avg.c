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

#ifdef GLOBAL_APPROACH
	sliding_avg_parms_t GSAP;

void intAverage(int16_t *avg, int16_t val)
{
	if (*avg == 0) {
		*avg = val << AvgShift(*avg);
	}
#ifdef WITH_PRECALCULATED_VALUE
	GSAP.value
#else
	int16_t value
#endif
			= AvgIntValue(*avg);
	GSAP.gradient = val -
#ifdef WITH_PRECALCULATED_VALUE
							GSAP.
#endif
								value;
	*avg += GSAP.gradient;
}

#else

void intAverage(sliding_avg_parms_t *avg, avg_value_t val)
{
	if (avg->sum == 0) {
//		avg->sum = val << AvgShift(*avg);
		avg->sum = AvgMultiply(val, *avg);
	}
#ifdef WITH_PRECALCULATED_VALUE
	avg->value = AvgIntValue(*avg);
#endif
#ifdef WITH_INDIVIDUAL_GRADIENT
	avg->gradient
#else
	_last_global_gradient
#endif
		= val -
#ifdef WITH_PRECALCULATED_VALUE
				avg->value;
#else
				AvgIntValue(*avg);
#endif



#ifdef WITH_AVG_ACCELERATION

//	int16_t acc_factor	= Gradient(*avg);

#if defined(WITH_ACC_FACTOR)
	avg->sum += Gradient(*avg);

	int16_t acc_factor = Gradient(*avg) * WITH_ACC_FACTOR;
	#define  	smooth_factor		2
#else
	#define		acc_factor			Gradient(*avg)
	#define  	smooth_factor		2

	avg->sum += acc_factor;
#endif
	int16_t gradAvgDelta = (acc_factor - avg->gradAvg/smooth_factor);
	avg->gradAvg += gradAvgDelta;

	int16_t accel = (avg->gradAvg - avg->lastGradAvg);
	int16_t accelDelta = (accel - avg->accel/smooth_factor);
	avg->accel += accelDelta;
	avg->lastGradAvg = avg->gradAvg;
#else
	avg->sum += Gradient(*avg);
#endif
}



#ifdef WITH_ACCELERATION_STRUCT
void intAccAverage(accel_avg_t *avg, int16_t val)
{
	intAverage(&avg->slider, val);
	intAverage(&avg->gradient, Gradient(avg->slider) );
//	intAverage(&avg->accel, Gradient(avg->gradient) );

/*

#if defined(WITH_ACC_FACTOR)
	int16_t acc_factor = Gradient(avg->slider) * WITH_ACC_FACTOR;
	#define  	smooth_factor		3
#else
	#define		acc_factor			Gradient(*avg)
	#define  	smooth_factor		2
#endif
	int16_t gradAvgDelta = (acc_factor - avg->gradAvg/smooth_factor);
	avg->gradAvg += gradAvgDelta;

	int16_t accel = (avg->gradAvg - avg->lastGradAvg);
	int16_t accelDelta = (accel - avg->accel/smooth_factor);
	avg->accel += accelDelta;
	avg->lastGradAvg = avg->gradAvg;
*/
}
#endif

#ifdef AVG_STABILIZER_THRESHOLD

#ifndef AVG_STABILIZER_ZERO
	#define		AVG_STABILIZER_ZERO		1
#endif

FORCE_INLINE
boolean stabilizedEvent(uint16_t abs_gradient,
			            uint16_t abs_accel,
			            avg_stabilizer_t *st)
{
	if (!st->isStable) {
		if ( (abs_gradient < AVG_STABILIZER_ZERO)
		  && (abs_accel < AVG_STABILIZER_ZERO) ) {
			st->isStable = true;
#ifdef WITH_STABILIZED_TIMESTAMPS
			st->delta_t = timer_data.now - st->lastEvent;
			st->lastEvent = timer_data.now;
#endif
			return true;
		}
	} else {
		if ((abs_gradient > AVG_STABILIZER_THRESHOLD)
		     && (abs_accel > AVG_STABILIZER_THRESHOLD)
	    ) {
			st->isStable = false;
#ifdef WITH_STABILIZED_TIMESTAMPS
			st->delta_t = timer_data.now - st->lastEvent;
			st->lastEvent = timer_data.now;
#endif
#ifdef WITH_STABILIZER_LEAVE_EVENT
			return true;
#endif
		}
	}
	return false;
}


FORCE_INLINE
boolean averageStabilizedEvent(sliding_avg_parms_t *avg
#ifdef WITH_AVG_ACCELERATION
#else
		, sliding_avg_parms_t *avg2
#endif
		,avg_stabilizer_t *stabi
		)
{
	return stabilizedEvent(
			 _ABS( Gradient(*avg) )
#ifdef WITH_AVG_ACCELERATION
			,_ABS( AvgAcceleration(*avg) )
#else
			,_ABS( Gradient(*avg2) )
#endif
			, stabi
			);
}

#endif //  AVG_STABILIZER_THRESHOLD


#endif


