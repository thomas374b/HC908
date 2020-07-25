/*
 * timer_hc908.h
 *
 *  Created on: 10.06.2020
 *      Author: pantec
 *
 *  timer functions to realize a system clock in small embedded systems
 *
 */

#ifndef TIMER_HC908_H_
#define TIMER_HC908_H_


#include "project.h"

#define		PREPRO_DIV256(x)		(x / 256L)
#define		PREPRO_DIV100(x)		(x / 100L)
#define		PREPRO_DIV2(x)			(x / 2L)


#ifndef TIMER_N
	#ifndef	_isrNo_TIMOVF
		#error "missing symbol _isrNo_TIMOVF, fix header file for CPU type"
	#else
		#define		TIMER_IRQ		_isrNo_TIMOVF	// number of overflow interrupt
	#endif

	#ifdef _MC68HC908MR32_H
		#define		TIMER_N			B

//		#define		TIMER_N			A
//		#define		TIMER_IRQ		_isrNo_TimAOvf
	#else
		#ifdef 	_MC68HC908JKJL_H
			#define		TIMER_N			1
		#else
			#error "no timer specified"
		#endif
	#endif
#endif

#ifdef WITH_TDELAY
	#ifndef 	CHANNEL_N
		#define		CHANNEL_N		1		// use channel 1 if not specified elsewhere
	#endif
#endif

#define	DEF_TREG(n)				DEF_CONCAT3(T,n,CNT)
#define	DEF_TREGH(n)			DEF_CONCAT3(T,n,CNTH)
#define	DEF_TSC(n)				DEF_CONCAT3(T,n,SC)
#define	DEF_TSC1(n)				DEF_CONCAT3(T,n,SC1)
#define	DEF_TMOD(n)				DEF_CONCAT3(T,n,MOD)

#ifdef WITH_TDELAY
	#define	DEF_TCHN(n, c)		DEF_CONCAT4(T,n,CH,c)
	#define	DEF_TCHNH(n, c)		DEF_CONCAT5(T,n,CH,c,H)
	#define	DEF_TCHNL(n, c)		DEF_CONCAT5(T,n,CH,c,L)
		// must also write to L register or timer is locked up

	#define	DEF_TSCH(n, c)		DEF_CONCAT5(T,n,CH,c,F)
	#define	DEF_TSOFT_BIT(n,c)	DEF_CONCAT5(T,n,MS,c,A)
#endif

#ifdef VERY_SIMPLE_TIMER
	// force 8bit
	#define		EIGHT_BIT_TIMER			1
	#undef		SIXTEEN_BIT_TIMER
#endif

#ifdef SIXTEEN_BIT_TIMER
	// !!! 32bit arithmetic adds about 1k code size
	typedef		uint16_t		timer_counter_t;
	typedef		uint32_t		timer_int_t;
	typedef		uint16_t		timer_gap_t;
	typedef		uint32_t		timer_frac_t;
	typedef		uint32_t		timer_wrap_t;

	#define 	timer_t_max		_UINT32_MAX

#if (!defined(OCR1A_MAX_VAL) || (OCR1A_MAX_VAL == 0))
		#error "need OCR1A_MAX_VAL declared"
#else
	#define		OCR_INTERNAL_VAL	OCR1A_MAX_VAL
#endif

	#define			TCNT_REG		DEF_TREG(TIMER_N)
	#ifdef WITH_TDELAY
		#define		TCHN_REG		DEF_TCHN(TIMER_N, CHANNEL_N)
	#endif
#else
	#define		EIGHT_BIT_TIMER			1

	typedef		uint8_t			timer_counter_t;
	typedef		uint16_t		timer_uint_t;
	typedef		int16_t			timer_int_t_;

	#define 	timer_t_max		_UINT16_MAX

	// TODO: review
	typedef		uint16_t		timer_gap_t;
	typedef		uint16_t		timer_frac_t;
	typedef		uint16_t		timer_wrap_t;

	#if (!defined(OCR1A_MAX_VAL) || (OCR1A_MAX_VAL == 0))
		#define		OCR_INTERNAL_VAL	256UL
	#else
		#if (OCR1A_MAX_VAL > 255)
			#define		OCR_INTERNAL_VAL	PREPRO_DIV256(OCR1A_MAX_VAL)
		#else
			#error "very short timer wrap, high interrupt load"
		#endif
	#endif

	#define			TCNT_REG		DEF_TREGH(TIMER_N)
	#ifdef WITH_TDELAY
		#define		TCHN_REG		DEF_TCHNH(TIMER_N, CHANNEL_N)
		#define		TCHN_REG_DUMMY	DEF_TCHNL(TIMER_N, CHANNEL_N)
	#endif
#endif


#define		TSC_REG		DEF_TSC(TIMER_N)
#define		TMOD_REG	DEF_TMOD(TIMER_N)

#ifdef WITH_TDELAY
	#define		TSOFT_BIT   DEF_TSOFT_BIT(TIMER_N, CHANNEL_N)
	#define		TSCH_FLAG	DEF_TSCH(TIMER_N, CHANNEL_N)
#endif


// TODO: rework

//#if (F_CPU > 4000000)
//	#define 	TIMER_PRESCALER_VALUE		128UL
//#else
	// TODO: check if timer is also driven by half frequency

	// timer prescaler is choosen to be as big as possible to minimize interrupt load
	#define 	TIMER_PRESCALER_VALUE		64UL
//#endif



typedef struct {
//optimized	timer_wrap_t 	n_timer_wraps_copy;		// snapshot of global time counter

	timer_wrap_t	wrap_count;			// global time counter since program start
	timer_counter_t tfrac;				// recent timer counter register snapshot
	timer_uint_t	now;				// actual time

#ifndef VERY_SIMPLE_TIMER
//	timer_uint_t	last_now;
#if (TIMER_WRAP != OCR_INTERNAL_VAL)
//	timer_frac_t	tmp;
#endif
#endif

#if defined(SIXTEEN_BIT_TIMER) || defined(TEN_BIT_TIMER)
	timer_counter_t 	tfrac_high;
#endif
} timer_data_t;


extern volatile	timer_data_t 	timer_data;


typedef struct {
	timer_uint_t gap;
	timer_uint_t nextEvent;
} timeout_t;

#ifdef WITH_GETTIME_FUNC
	extern	timer_uint_t		getTime(void);
#else
	#ifdef WITH_MDELAY
		extern	void			getTime(void);
	#endif
#endif


#if (TIMER_WRAP & 0x01)
	// no optimization possible
	#define		TIMER_WRAP_D		1
#else
#if (TIMER_WRAP & 0x03)
	#define		TIMER_WRAP_D		2
#else
#if (TIMER_WRAP & 0x07)
	#define		TIMER_WRAP_D		4
#else
#if (TIMER_WRAP & 0x0F)
	#define		TIMER_WRAP_D		8
#else
#if (TIMER_WRAP & 0x1F)
	#define		TIMER_WRAP_D		16
#else
#if (TIMER_WRAP & 0x3F)
	#define		TIMER_WRAP_D		32
#else
	#warning "TIMER_WRAP_D need optimization constant"
	#define		TIMER_WRAP_D		1
#endif
#endif
#endif
#endif
#endif
#endif

#define		TIMER_WRAP_M 		(TIMER_WRAP/TIMER_WRAP_D)
#define		TIMER_WRAP_F		(256/TIMER_WRAP_D)
#define		TIMER_WRAP_F2		(TIMER_WRAP_F/2)

#if ((TIMER_WRAP_M * 255) > 65535)
	#warning "TIMER_WRAP too large, need 32bit calculations"
#endif


#ifdef WITH_GETTIME_FUNC
timer_uint_t
#else
// inline
void
#endif
getTime(void)
{
	timer_counter_t tfrac;

	// this is a problematic area, a critical region
	// but we don't want make use of sei/cli since this would skew the timer clock

	while(1) {
		// double read approach
		tfrac = TCNT_REG;

		timer_data.now = timer_data.wrap_count;
		timer_data.tfrac = tfrac;

		tfrac = TCNT_REG;

		if (tfrac < timer_data.tfrac) {
			// has wrapped
//			timer_data.counter_wrapped = true;
#ifdef WITH_COP_ENABLED
			COP_Reset();
#endif
			// introduces ??? ms jitter @ wrap
			continue;	// in rare cases we do this activity twice
		}
		break;
	}
	// we have a snapshot of "global_timer_wraps" in "now"

#ifdef VERY_SIMPLE_TIMER
#if (OCR1A_MAX_VAL == 0)
	timer_data.now <<= 8;
#else
/*
	timer_counter_t mulLoopCnt = (OCR_INTERNAL_VAL-1) & 0xFF;
	timer_uint_t na = timer_data.now;
	do {
		timer_data.now += na;
		mulLoopCnt--;
	} while (mulLoopCnt > 0);
*/
	timer_data.now *= OCR_INTERNAL_VAL;
#endif
	timer_data.now += timer_data.tfrac;

#else
	// precise timing calculation
	timer_data.now *= TIMER_WRAP;

	// Compiler produziert Schund, Zahlen werden größer als 16bit, keine Warnung
	timer_frac_t tmp = timer_data.tfrac;
	tmp *= TIMER_WRAP_M;
#if (OCR_INTERNAL_VAL == 256)
	tmp += TIMER_WRAP_F2;
	tmp /= TIMER_WRAP_F;
//	tmp >>= 5;
#else
#if ((OCR_INTERNAL_VAL & 0x0F) != 0)
	timer_data.tmp += (OCR_INTERNAL_VAL/2);  // richtig runden
#endif
	timer_data.tmp /= OCR_INTERNAL_VAL;
#endif
	timer_data.now += tmp;
#endif


#ifdef WITH_GETTIME_FUNC
	return timer_data.now;
#endif
}

#define HAVE_TOVF_ISR		1
void timOvfIrq(void) __interrupt(TIMER_IRQ)
{
	TSC_REG &= ~0x80;	// clear interrupt flag

	timer_data.wrap_count++;

#if defined(CLOCKPIN_MASK)
	if (CLOCKPIN > 0) {
		CLOCKPIN = 0;
	} else {
		CLOCKPIN = 1;
	}
#endif
}


#pragma save
#pragma nooverlay
/*
 * fires every @gap milliseconds
 */
uint8_t at_timer_interval(const timer_uint_t gap, timer_uint_t *expires) // reentrant
//uint8_t at_timer_interval(timeout_t *timer) // reentrant
// should be in ms -1
{
#ifndef WITH_SLOTTED_GETTIME
	getTime();
#endif

	timer_int_t_ delta = (timer_int_t_)timer_data.now;
	uint8_t fire = false;

	if (		// avoid too much calculations
		   (((*expires & 0x8000) > 0) && ((timer_data.now & 0x8000) == 0))
		|| (((*expires & 0x8000) == 0) && ((timer_data.now & 0x8000) > 0))   )
	{
		// 'now' and 'expires' lay on opposite sides of timer wrap, we must subtract

		delta -= (timer_int_t_)*expires;

		if ((delta > 0) && (delta < 32766)) {
			// timer expired, remind! we cant have gaps larger than (TIMER_WRAP*32767)
			// gaps larger than 32767 will jitter or be missed when they fall close to timer wrap
			fire = true;
		}
	} else {
		// both numbers can be compared directly
		if (timer_data.now > *expires) {
			delta -= (timer_int_t_)*expires;	// delta has the overdue time
			fire = true;
		}
	}
	if (fire) {
		*expires = timer_data.now;
		*expires += gap;
		*expires -= delta;				// calculate next timeout

#ifdef WITH_COP_ENABLED
		// caller is going to execute extra stuff, make room
		COP_Reset();
#endif
		return true;
	}

	return false;
}
#pragma restore



#ifdef VERY_SIMPLE_TIMER
	#define 	MS_TO_GAP(x)		(((x * OCR_INTERNAL_VAL)  + (TIMER_WRAP/2)) / TIMER_WRAP)

	#define		OCR_TO_TICKS_DIV	PREPRO_DIV256(OCR1A_MAX_VAL)
	#define		TICKS_ROUNDUP		PREPRO_DIV2(OCR_TO_TICKS_DIV)
	#define 	TICKS_TO_MS(x)		(((x * TIMER_WRAP)  + TICKS_ROUNDUP) / OCR_TO_TICKS_DIV)

	#define 	at_interval(x,y)		at_timer_interval(MS_TO_GAP(x),  y)
#else
	#define 	at_interval(x,y)		at_timer_interval(x,  y)
#endif


#ifdef WITH_UDELAY
void uDelay(uint16_t a)
{
//	a >>= 1;
	while (--a != 0) {
		// 1x nop == 0.5 µsec @ 2 MHz
		__asm
			nop
		__endasm;
	};
}
#endif


#ifdef WITH_MDELAY
void mDelay(uint16_t a)
{
#ifdef WITH_GETTIME_FUNC
	unsigned long t0 = getTime();
	unsigned long t1 = t0 + a;
	unsigned long t = t0;
	while (t < t1) {
		t = getTime();
	}
#else
	// TODO: handle timer wrap
	getTime();
	timer_int_t t = timer_data.now + a;
	do {
		getTime();
	} while (timer_data.now < t);
#endif
}
#endif


inline
void timer_init(void)
{
//	memset((void*)&timer_data, 0, sizeof(timer_data_t));
/*
	timer_data.counter_wrapped = false;
	timer_data.global_timer_wraps = 0;
	timer_data.now = 0;
*/
#if defined(CLOCKPIN_DDR)
	CLOCKPIN_DDR |= CLOCKPIN_MASK;
#endif

#if (defined(OCR1A_MAX_VAL) && (OCR1A_MAX_VAL < 65536) && (OCR1A_MAX_VAL != 0))
	TMOD_REG = (OCR1A_MAX_VAL -1);
#else
	TMOD_REG = 65535;
#endif
#ifdef WITH_TDELAY
    TSOFT_BIT = 1;			// software output compare without toggling wires
#endif

	TSC_REG |= 0x40 | 0x06;	// TOIE & prescaler 1/64
	TSC_REG &= ~_BV(5);		// clear TSTOP
}

#ifdef WITH_TDELAY
	#define 		gaptimer_expired()					(TSCH_FLAG > 0)

	extern void tDelay(timer_counter_t gap);
	extern void setup_gaptimer(timer_counter_t gap);


void setup_gaptimer(timer_counter_t gap)
{
        timer_counter_t next = TCNT_REG + gap;
        	// should subtract needed instruction cycles also

#if defined(OCR1A_MAX_VAL)
#if (OCR1A_MAX_VAL != 0)		// adjust for timer roll-over
        if (next > OCR_INTERNAL_VAL) {
                next -= OCR_INTERNAL_VAL;
        }
#endif
#endif
         // clear flag on Timer
        TSCH_FLAG = 0;

        // set next expiration
        TCHN_REG = next;
        TCHN_REG_DUMMY = 0;		// must read also the low byte or the timer remains locked/stopped
}


void tDelay(timer_counter_t gap)
{
        setup_gaptimer(gap);

        while( ! gaptimer_expired() );
}

#endif


#endif /* TIMER_HC908_H_ */
