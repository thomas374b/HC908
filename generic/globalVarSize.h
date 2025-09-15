/*
 * globalVarSize.h
 *
 *  Created on: 22.07.2020
 *      Author: pantec
 */

#ifndef GLOBALVARSIZE_H_
#define GLOBALVARSIZE_H_

typedef struct {

	#include	"globalVars.h"

} global_vars_t;

#define		GLOBAL_VARS_SIZE	sizeof(global_vars_t)


#endif /* GLOBALVARSIZE_H_ */
