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

#ifdef GLOBAL_APPROACH
	#define AvgSum(x)				(x)
	#define AvgFloatValue(x,y)   	(((myFloat)x) / y)
	#define isNegative(x)			(x <= 0)
	#define Gradient(x)				(GSAP.gradient)


	#ifdef WITH_AVG_SHIFTING
			#ifdef WITH_FLEXIBLE_AVG_LEN
				#define AvgShift(x)			GSAP.shift
			#else
				#define AvgShift(x)			5
			#endif

			#define AvgIntValue(x)			(x >> AvgShift(x))
	#else
			#ifdef WITH_FLEXIBLE_AVG_LEN
				#define AvgDivider(x)			GSAP.divider
			#else
				#define AvgDivider(x)			32
			#endif

			#define AvgIntValue(x)			(x / AvgDivider(x))
	#endif

#else


	#ifdef WITH_FLEXIBLE_AVG_LEN
		#define AvgDivider(x)			((x).divider)
		#define AvgShift(x)				((x).shift)
	#else
		#define AvgShift(x)			(FIXED_AVG_SHIFT)

		#ifndef FIXED_AVG_SHIFT
			#define FIXED_AVG_SHIFT		5
			#define AvgDivider(x)		32
			#define AvgFloatDivider		32.0
		#endif
	#endif

	#ifdef WITH_AVG_SHIFTING
		#define AvgMultiply(v, x)	(v << (x).shift)
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
		#ifdef WITH_AVG_SHIFTING
			#define AvgIntValue(x)		((x).sum >> AvgShift(x))
		#else
			#define AvgIntValue(x)		((x).sum / AvgDivider(x))
		#endif
	#endif

// #define AvgFloatValue(x)		((myFloat)((x).sum))
	#define AvgFloatValue(x,y)   	(((myFloat)(x).sum) / y)
	#define isNegative(x)			((x).sum <= 0)


	#if defined(WITH_AVG_ACCELERATION) || defined(WITH_ACCELERATION_STRUCT)
		#define AvgAcceleration(x)		((x).accel)
	#endif

#endif

typedef struct {
	uint8_t isStable;
#ifdef WITH_STABILIZED_TIMESTAMPS
	uint16_t delta_t;
	uint16_t lastEvent;
#endif
} avg_stabilizer_t;



typedef struct {
#ifdef GLOBAL_APPROACH
	int16_t gradient;
#ifdef WITH_FLEXIBLE_AVG_LEN
#ifdef WITH_AVG_SHIFTING
	uint8_t shift;
#else
	uint8_t divider;
#endif
#endif
#ifdef WITH_PRECALCULATED_VALUE
	int16_t value;
#endif
#else
#ifdef WITH_SHORT_AVG_CALC
	int16_t sum;
#else
#ifdef WITH_PRECALCULATED_VALUE
	avg_value_t value;
#endif
	int32_t
				sum;

#endif

#ifdef WITH_INDIVIDUAL_GRADIENT
	int16_t 	gradient;
#endif

#ifdef WITH_FLEXIBLE_AVG_LEN
	#ifdef WITH_AVG_SHIFTING
		uint8_t shift;
	#else
		uint8_t divider;
	#endif
#endif


#ifdef WITH_AVG_ACCELERATION
	int16_t accel;
	int16_t gradAvg;
	int16_t lastGradAvg;
#endif
#endif
} sliding_avg_parms_t;

#ifdef AVG_STABILIZER_THRESHOLD



extern boolean averageStabilizedEvent(sliding_avg_parms_t *avg
#ifdef WITH_AVG_ACCELERATION
#else
		, sliding_avg_parms_t *avg2
#endif
		,avg_stabilizer_t *stabi
		);
#endif


#ifdef WITH_ACCELERATION_STRUCT
typedef struct {
/*
	int16_t accel;
	int16_t gradAvg;
	int16_t lastGradAvg;
*/
	sliding_avg_parms_t slider;
	sliding_avg_parms_t gradient;
//	sliding_avg_parms_t accel;
} accel_avg_t;
#endif



#ifdef GLOBAL_APPROACH
	extern sliding_avg_parms_t	GSAP;
	extern void intAverage(int16_t *old, int16_t val);
#else
	extern void intAverage(sliding_avg_parms_t *old, int16_t val);
#endif

#endif /* SLIDING_INT_AVG_H_ */
