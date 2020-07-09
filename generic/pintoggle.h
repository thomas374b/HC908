/*
 * pintoggle.h
 *
 *  Created on: 15.12.2015
 *      Author: pantec
 */

#ifndef PINTOGGLE_H_
#define PINTOGGLE_H_

#define		pin_high(_PORT, _PIN_MASK) 			{ _PORT |= _PIN_MASK; }
#define 	pin_low(_PORT, _PIN_MASK)			{ _PORT &= ~_PIN_MASK; }
#define		pin_toggle(_PORT, _PIN_MASK)		{ _PORT ^= _PIN_MASK;  }
// #define		pin_toggle(_PIN, _PIN_MASK)			{ _PIN |= _PIN_MASK;  }

#define 	pin_isSet(_PORT, _PIN_MASK)			((_PORT & _PIN_MASK) > 0)


#endif /* PINTOGGLE_H_ */
