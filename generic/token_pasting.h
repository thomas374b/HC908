/*
 * token_pasting.h
 *
 *  Created on: 01.06.2020
 *      Author: pantec
 *
 *   tricky preprocessor macros to generalize pin and flag definitions
 *   and to help having only one single identifying symbol for a pin
 *
 */

#ifndef TOKEN_PASTING_H_
#define TOKEN_PASTING_H_


	#define DEF_CONCAT(a, b)            a ## b
	#define DEF_CONCAT3(x, y, z)        x ## y ## z
	#define DEF_CONCAT4(i, j, k, l)		i ## j ## k ## l
	#define DEF_CONCAT5(m, n, o, p, q)  m ## n ## o ## p ## q

	#define DEF_CONCAT_EXPANDED(a, b)   DEF_CONCAT(a, b)

	#define	DEF_DDR(name)				DEF_CONCAT(DDR, name)
	#define	DEF_PORT(name)				DEF_CONCAT(PORT, name)
	#define	DEF_PIN(name)				DEF_CONCAT(PIN, name)
	#define	DEF_P(name)					DEF_CONCAT(P, name)
	#define	DEF_PX(name, num)			DEF_CONCAT3(P, name, num)
	#define	DEF_PTX(name, num)			DEF_CONCAT3(PT, name, num)



	#define	DEF_INT(name, num)			DEF_CONCAT(name, num)
	#define	DEF_INTF(num)				DEF_CONCAT(INTF, num)

	#define	DEF_TIFR(x)					DEF_CONCAT(TIFR, x)
	#define	DEF_OCF(x,y)				DEF_CONCAT3(OCF,x,y)
	#define	DEF_OCR(x,y)				DEF_CONCAT3(OCR,x,y)
	#define	DEF_WGM(x,y)				DEF_CONCAT3(WGM,x,y)
	#define	DEF_COM(x,y,z)				DEF_CONCAT4(COM,x,y,z)

#if defined(__AVR_ATtiny84A__) || defined(__AVR_ATtiny85__)
//	#define  __TIM						TIM

	#ifdef __AVR_ATtiny85__
		// tiny85 has only one TIMSK
		#define	DEF_TIMSK(x)				TIMSK
	#endif

	#define	DEF_TVECT(x,y)				DEF_CONCAT5(TIM, x, _COMP, y, _vect)
	#define	DEF_TVECT_NUM(x,y)			DEF_CONCAT5(TIM, x, _COMP, y, _vect_num)
	#define DEF_TVECT_OVF(x)			DEF_CONCAT3(TIM, x, _OVF_vect)

#else
//	#define  __TIM						TIMER
	#if defined(__AVR_ATtiny4313__) || defined(__AVR_ATtiny2313__)
		#define	DEF_TIMSK(x)				TIMSK
	#else
		#define	DEF_TIMSK(x)				DEF_CONCAT(TIMSK, x)
	#endif

	#define	DEF_TVECT(x,y)				DEF_CONCAT5(TIMER, x, _COMP, y, _vect)
	#define	DEF_TVECT_NUM(x,y)			DEF_CONCAT5(TIMER, x, _COMP, y, _vect_num)
	#define DEF_TVECT_OVF(x)			DEF_CONCAT3(TIMER, x, _OVF_vect)
#endif




//	TIMER0_COMPA_vect
// #if defined(__AVR_ATtiny85__) || defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega8__)  || defined(__AVR_ATmega328P__) ||  || defined(__AVR_ATtiny861__) || defined(__AVR_ATtiny861A__)
//	#define	VEC_TIM						TIM
//	#define	DEF_TVECT(x,y)				DEF_CONCAT5(TIMER, x, _COMP, y, _vect)
/*
 *  maybe from older compiler/header files
#else
//	#define	VEC_TIM						TIMER
	#define	DEF_TVECT(x,y)				DEF_CONCAT5(TIM, x, _COMP, y, _vect)
#endif
*/

	#define	DEF_OCIE(x,y)				DEF_CONCAT3(OCIE,x,y)
	#define	DEF_TCNT(x)					DEF_CONCAT(TCNT,x)

#if defined(__AVR_ATmega8__)
	#define	DEF_TICIE(x)				DEF_CONCAT(TICIE, x)
#else
	#define	DEF_TICIE(x)				DEF_CONCAT(ICIE, x)
#endif

#if defined(__AVR_ATtiny85__) || defined(__AVR_ATtiny861__) || defined(__AVR_ATtiny861A__)
	#define	DEF_TIMSK__(x)			TIMSK
#else
	#define	DEF_TIMSK__(x)			DEF_TIMSK(x)
#endif

#if defined(__AVR_ATtiny85__) || defined(__AVR_ATtiny861__) || defined(__AVR_ATtiny861A__)
	#define	DEF_TIFR__(x)			TIFR
#else
	#define	DEF_TIFR__(x)			DEF_TIFR(x)
#endif

#if defined(__AVR_ATtiny861__) || defined(__AVR_ATtiny861A__)
	#define	DEF_TCNTL(x)			DEF_CONCAT3(TCNT,x,L)
#else
	#define	DEF_TCNTL(x)			DEF_TCNT(x)
#endif

	#define		DEF_INT_VECT(x,y)		DEF_CONCAT3(x, y, _vect)

	#define		DEF_PCMSK(x)			DEF_CONCAT(PCMSK, x)
	#define		DEF_PCIE(x)				DEF_CONCAT(PCIE, x)
	#define		DEF_PCIF(x)				DEF_CONCAT(PCIF, x)
	#define		DEF_PCINT(x)			DEF_CONCAT(PCINT, x)


	#define	EVALUATE(x)					x
	#define	STRINGIFY(x)				#x


	// macros to compare single char definitions from A..H
	#define		_def_A		1
	#define		_def_B		2
	#define		_def_C		3
	#define		_def_D		4
	#define		_def_E		5
	#define		_def_F		6
	#define		_def_G		7
	#define		_def_H		8

	#define		_num_1		-1
	#define		_num_2		-2
	#define		_num_3		-3
	#define		_num_4		-4
	#define		_num_5		-5
	#define		_num_6		-6
	#define		_num_7		-7
	#define		_num_8		-8


	#define		DEF_UNDERSCORED(char)	DEF_CONCAT(_def_, char)
	#define		NUM_UNDERSCORED(num)	DEF_CONCAT(_num_, num)
	#define		DEF_VECTORED(num)		DEF_CONCAT(__vector_, num)

	#define		IS_SAME_CHAR(x,y)		(DEF_UNDERSCORED(x) == DEF_UNDERSCORED(y))
	#define		IS_SAME_NUM(x,y)		(NUM_UNDERSCORED(x) == NUM_UNDERSCORED(y))

/*
	#define		DEF_BIT_CHAR(x,y)		\
		##define		DEF_CONCAT(x, _BIT_NO)		DEF_CONCAT(y, _BIT)		\
		##define		DEF_CONCAT(x, _PORT_NO)		DEF_CONCAT(y, _CHAR)
*/


#endif /* TOKEN_PASTING_H_ */
