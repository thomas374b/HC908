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


	#define	DEF_VECT(t,n)				DEF_CONCAT3(t, n, _vect)
	#define	DEF_INT(name, num)			DEF_CONCAT(name, num)
	#define	DEF_INTF(num)				DEF_CONCAT(INTF, num)

	#define	DEF_TMASK(x)				DEF_CONCAT(TIMSK, x)
	#define	DEF_TIFR(x)					DEF_CONCAT(TIFR, x)
	#define	DEF_OCF(x,y)				DEF_CONCAT3(OCF,x,y)
	#define	DEF_OCR(x,y)				DEF_CONCAT3(OCR,x,y)
	#define	DEF_TVECT(x,y)				DEF_CONCAT5(TIMER,x,_COMP,y,_vect)
	#define	DEF_OCIE(x,y)				DEF_CONCAT3(OCIE,x,y)
	#define	DEF_TCNT(x)					DEF_CONCAT(TCNT,x)


	#define	EVALUATE(x)					x
	#define	STRINGIFY(x)				#x


#endif /* TOKEN_PASTING_H_ */
