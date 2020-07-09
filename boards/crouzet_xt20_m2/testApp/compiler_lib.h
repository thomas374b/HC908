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

#endif /* COMPILER_LIB_H_ */
