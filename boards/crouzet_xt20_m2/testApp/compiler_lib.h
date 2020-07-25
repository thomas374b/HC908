/*
 * compiler_lib.h
 *
 *  Created on: 24.06.2020
 *      Author: pantec
 *
 *
 *    the compiler/linker is very buggy, one needs to include the
 *    needed library files directly, to get them recompiled with
 *    the correct CFLAGS, if you just plain link you cant multiply
 *
 *    you should name --nostdlib in your Makefile to avoid the
 *    precompiled library get linked
 *
 *    the below stated files are symlinks to the stdlib functions
 *    from the SDCC compiler sources
 *
 */

#ifndef COMPILER_LIB_H_
#define COMPILER_LIB_H_


#include "_mulint.c"
#include "_divuint.c"
#include "_moduint.c"
#include "_modsint.c"
#include "_mullong.c"
#include "_divulong.c"

__data unsigned char __SDCC_hc08_ret2;
__data unsigned char __SDCC_hc08_ret3;

#include <stdint.h>


#if 0



union _bil {
        struct {unsigned char b3,b2,b1,b0 ;} b;
        struct {unsigned short hi,lo ;} i;
        unsigned long l;
        struct { unsigned char b3; unsigned short i12; unsigned char b0;} bi;
};

union _bis {
        struct {unsigned char b1,b0 ;} b;
        unsigned short s;
};


#define _acast(x) ((union _bil _AUTOMEM *)(x))
#define _bcast(x) ((union _bis _AUTOMEM *)&(x))

/*
                     3   2   1   0
       X                     1   0
       ----------------------------
                   0.3 0.2 0.1 0.0
               1.3 1.2 1.1 1.0

					D	C	B	A
                H   G   F   E

                    3   2   1   0
*/
#endif
#if 0
_mullong16 (long *a, int b)
{
        union _bil t;

//        t.i.hi = _acast(a)->b.b0 * _bcast(b)->b.b2;           // A
        t.i.hi = 0;
        t.i.lo = _acast(a)->b.b0 * _bcast(b)->b.b0;           // A
        t.b.b3 += _acast(a)->b.b3 * _bcast(b)->b.b0;          // G
        t.b.b3 += _acast(a)->b.b2 * _bcast(b)->b.b1;          // F
        t.i.hi += _acast(a)->b.b2 * _bcast(b)->b.b0;          // E <- b lost in .lst
        // bcast(a)->i.hi is free !
        t.i.hi += _acast(a)->b.b1 * _bcast(b)->b.b1;          // D <- b lost in .lst

//        _acast(a)->bi.b3 = _acast(a)->b.b1 * _bcast(b)->b.b2;  // C
        _acast(a)->bi.i12 = _acast(a)->b.b1 * _bcast(b)->b.b0; // C

//        _bcast(b)->bi.b3 = _acast(a)->b.b0 * _bcast(b)->b.b3;  // B
        _bcast(b)->bi.i12 = _acast(a)->b.b0 * _bcast(b)->b.b1; // B
        _bcast(b)->bi.b0 = 0;                                // B

        _acast(a)->bi.b0 = 0;                                // C
        t.l += a;

        return t.l + b;
}
#endif
#if 0
void _32mul16(uint32_t *a, uint16_t b)
{
	uint16_t A,B,C,D;

	uint32_t result;

	A = _acast(a)->b.b0 * _bcast(b)->b.b0;
	B = _acast(a)->b.b1 * _bcast(b)->b.b0;
	C = _acast(a)->b.b2 * _bcast(b)->b.b0;
	D = _acast(a)->b.b3 * _bcast(b)->b.b0;

	_acast(&result)->b.b0 = _bcast(A)->b.b0;
	_acast(&result)->b.b1 = _bcast(B)->b.b0 + _bcast(A)->b.b1;
	_acast(&result)->b.b2 = _bcast(C)->b.b0 + _bcast(B)->b.b1;
	_acast(&result)->b.b3 = _bcast(D)->b.b0 + _bcast(C)->b.b1;

	A = _acast(a)->b.b0 * _bcast(b)->b.b1;
	B = _acast(a)->b.b1 * _bcast(b)->b.b1;
	C = _acast(a)->b.b2 * _bcast(b)->b.b1;

	_acast(a)->b.b0 = _acast(&result)->b.b0;
	_acast(a)->b.b1 = _acast(&result)->b.b1;
	_acast(a)->b.b2 = _acast(&result)->b.b2;
	_acast(a)->b.b3 = _acast(&result)->b.b3;

	_acast(a)->b.b1 += _bcast(A)->b.b0;
	_acast(a)->b.b2 += _bcast(B)->b.b0   +    _bcast(A)->b.b1;
	_acast(a)->b.b3 += _bcast(C)->b.b0   +    _bcast(B)->b.b1;
//	_acast(a)->b.b4 += /*_bcast(D)->b.b0 + */ _bcast(C)->b.b1;
//	_acast(a)->b.b5 += /*                     _bcast(D)->b.b1*/;
}
#endif

#endif /* COMPILER_LIB_H_ */
