/*
 * types_hc908.h
 *
 *  Created on: 10.06.2020
 *      Author: pantec
 */

#ifndef TYPES_HC908_H_
#define TYPES_HC908_H_


#include <stdio.h>
#include <stdint.h>

typedef		uint8_t		boolean_t;

#if !defined(boolean)
// some compiler can do this, some not, some even don't know what true and false is
#if !defined(false)
	typedef enum {
		false = 0,
		true = 1
	} boolean;
#else
	typedef enum {
		bFalse = false,
		bTrue = true
	} boolean;
#endif
#endif

#define	_BV(x)		(1 << x)
#define	_ABS(x)		(((x)>0)?(x):(-(x)))


#define		_UINT8_MAX		((1 <<  8) -1)
#define		_UINT16_MAX		((1 << 16) -1)
#define		_UINT24_MAX		((1 << 24) -1)
#define		_UINT32_MAX		((1 << 32) -1)

#endif /* TYPES_HC908_H_ */
