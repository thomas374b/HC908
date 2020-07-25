/*
 * sliding_int_avg.h
 *
 *  Created on: 08.01.2018
 *      Author: pantec
 */

#ifndef SLIDING_INT_AVG_H_
#define SLIDING_INT_AVG_H_


#include "myFeatures.h"
#include "myTypes.h"
#include <stdint.h>

#ifdef WITH_FREEZING_AVG

typedef struct {
	int16_t sum;
	uint8_t cnt;
} freezing_int_avg_t;

#ifdef FREEZING_FUNC_PROTOS

void reset_freezing(int16_t val, freezing_int_avg_t *avg)
{
	avg->sum = val;
	avg->cnt = 1;
}

void add_freezing(int16_t val, freezing_int_avg_t *avg)
{
	avg->sum += val;
	avg->cnt++;
}

#else
	#define reset_freezing(v,a)		{ a.sum = v; a.cnt = 1; }
	#define add_freezing(v,a)		{ if (a.cnt < 32) { a.cnt++; } else { a.sum -= a.sum/32; }  a.sum += v; }
	#define freezing_val(a)			( a.sum / a.cnt )
#endif
/*
void add_freezing(int16_t v, freezing_int_avg_t *a)
{
	if (a->cnt < 32) {
		a->cnt++;
	} else {
		a->sum -= a->sum/32;
	}
	a->sum += v;
}
*/
/*
void reset_freezing(int16_t v, freezing_int_avg_t *a)
{
	a->sum = v;
	a->cnt = 1;
}
*/
#endif


//#ifdef WITH_SHORT_AVG_CALC
//	#ifdef WITH_PRECALCULATED_VALUE
//		#warning "WITH_PRECALCULATED_VALUE conflicts with WITH_SHORT_AVG_CALC"
//		#undef WITH_PRECALCULATED_VALUE
//	#endif
//#endif

#ifdef 	WITH_24BIT_AVG_VALUES
	#define		avg_value_t		int32_t
/*
	#ifndef WITH_PRECALCULATED_VALUE
		#define WITH_PRECALCULATED_VALUE
	#endif
*/
#else
	#define		avg_value_t		int16_t
#endif


#ifdef WITH_FLEXIBLE_AVG_LEN
	#define AvgDivider(x)			((x).divider)
	#define AvgShift(x)				((x).shift)
#else
	#ifndef FIXED_AVG_SHIFT
		#define FIXED_AVG_SHIFT		5
		#define AvgDivider(x)		32
		#define AvgDivider2(x)		16
		#define AvgFloatDivider		32.0
	#else
		#define AvgDivider(x)		(1 << FIXED_AVG_SHIFT)
		#define AvgDivider2(x)		(1 << (FIXED_AVG_SHIFT-1))
	#endif
	#define AvgShift(x)				(FIXED_AVG_SHIFT)
#endif

#ifdef WITH_AVG_SHIFTING
	#define AvgMultiply(v, x)	(v << AvgShift(x))
#else
	#define AvgMultiply(v, x)	(v * AvgDivider(x))
#endif

#ifdef WITH_INDIVIDUAL_GRADIENT
	#define Gradient(x)		((x).gradient)
#else
	extern   int16_t		_last_global_gradient;
	#define Gradient(x)		(_last_global_gradient)
#endif


#ifdef WITH_SHORT_AVG_CALC
	#define AvgSum(x)			((x).sum)
#else
	#ifdef WITH_PRECALCULATED_VALUE
		#define AvgSum(x)		((x).value)
	#else
		#define AvgSum(x)		((int16_t)((x).sum >> AvgShift(x)))
	#endif
#endif

#ifdef WITH_SHORT_AVG_CALC
	#ifdef WITH_AVG_SHIFTING
		#define AvgIntValue(x)		(AvgSum(x) >> AvgShift(x))
	#else
		#define AvgIntValue(x)		(AvgSum(x) / AvgDivider(x))
	#endif
#else
	#ifdef WITH_FLEXIBLE_AVG_LEN
		#define AvgIntValue(x)			divide(x.sum, AvgShift(x));
	#else
		#define AvgIntValue(x)			divide(x.sum);
	#endif
#endif

// #define AvgFloatValue(x)		((myFloat)((x).sum))
	#define AvgFloatValue(x,y)   	(((myFloat)(x).sum) / y)
	#define isNegative(x)			((x).sum <= 0)

#if defined(WITH_AVG_ACCELERATION)
	#define AvgAcceleration(x)		((x).accel)
#endif



#ifdef WITH_NEW_STABILIZER_CODE

typedef struct {
	uint8_t isBraking;
	uint8_t isStable;
	uint16_t rms;
	uint16_t threshold;
#ifdef WITH_FREEZING_AVG
	freezing_int_avg_t freezer;		// short term average
#endif
#ifdef WITH_STABILIZED_TIMESTAMPS
	uint16_t delta_t;
	uint16_t lastEvent;
#endif
} avg_stabilizer_t;

#endif


typedef struct {

#ifdef WITH_PRECALCULATED_VALUE
	int16_t value;
#endif

#ifdef WITH_SHORT_AVG_CALC
	int16_t 	sum;
#else
#ifdef WITH_PRECALCULATED_VALUE
	avg_value_t value;
#endif
	int32_t		sum;
#endif

#ifdef WITH_INDIVIDUAL_GRADIENT
	int16_t 	gradient;
#endif

#ifdef WITH_TRACE_MIN_MAX
	int16_t min,max;
#endif

#ifdef WITH_FLEXIBLE_AVG_LEN
	#ifdef WITH_AVG_SHIFTING
		uint8_t shift;
	#else
		uint8_t divider;
	#endif
#endif

#ifdef WITH_AVG_ACCELERATION
	int16_t accelAvg;
	int16_t gradAvg;
#endif
#ifdef WITH_NEW_STABILIZER_CODE
	int16_t aDelta;
#endif
} sliding_avg_parms_t;



#ifdef WITH_ACCELERATION_STRUCT
typedef struct {
	sliding_avg_parms_t slider;
	sliding_avg_parms_t gradient;
} accel_avg_t;
#endif


extern void	intAverage(sliding_avg_parms_t *old, int16_t val);
extern int16_t divide(int16_t v);

#ifdef WITH_NEW_STABILIZER_CODE
extern uint8_t averageStabilizedEvent(sliding_avg_parms_t *, avg_stabilizer_t *);		// return true if stabilized or false if floating
#endif

#endif /* SLIDING_INT_AVG_H_ */
