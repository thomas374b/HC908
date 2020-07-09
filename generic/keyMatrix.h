/*
 * keyMatrix.h
 *
 *  Created on: 22.06.2020
 *      Author: pantec
 *
 *  code for a 2x3 keymap
 *     - bits on columns are inputs and pulled up with resistors
 *     - bit on rows are outputs
 *     - to check a key on a row this row must be pulled down while the other should float high
 *
 *
 *
 */


#ifndef KEYMATRIX_H_
#define KEYMATRIX_H_

#include "project.h"		// a container for all necessary includes

#ifdef		WITH_BITWISE_DOUBLE_KEY
	#define NUM_KEYS		12
#else
	#define NUM_KEYS		6
#endif


// CAVEAT! not all ill combinations are covered by this assert
#if (BIT_ROW0_NUM == BIT_COL0_NUM) || (BIT_COL0_NUM == KEY_PRESS_BIT)
	#error "row 0 and col 0 cannot share the same bit, review your design"
#endif

#if (BIT_ROW0_NUM == BIT_COL1_NUM) || (BIT_COL1_NUM == KEY_PRESS_BIT)
	#error "row 0 and col 1 cannot share the same bit, review your design"
#endif

#if (BIT_ROW0_NUM == BIT_COL2_NUM) || (BIT_COL2_NUM == KEY_PRESS_BIT)
	#error "row 0 and col 2 cannot share the same bit, review your design"
#endif

#if (BIT_ROW1_NUM == BIT_COL0_NUM) || (BIT_ROW0_NUM == KEY_RELEASE_BIT)
	#error "row 1 and col 0 cannot share the same bit, review your design"
#endif

#if (BIT_ROW1_NUM == BIT_COL1_NUM) || (BIT_ROW1_NUM == KEY_RELEASE_BIT)
	#error "row 1 and col 1 cannot share the same bit, review your design"
#endif

#if (BIT_ROW1_NUM == BIT_COL2_NUM) || (BIT_ROW2_NUM == KEY_RELEASE_BIT)
	#error "row 1 and col 2 cannot share the same bit, review your design"
#endif

/*
#define		BIT_ROW_EN_DDR		DEF_DDR(BIT_ROW_EN_CHAR)
#define		BIT_ROW_EN_PORT		DEF_PORT(BIT_ROW_EN_CHAR)
#define		BIT_COL0			DEF_PTX(BIT_COL_CHAR, BIT_COL0_NUM)
#define		BIT_COL1			DEF_PTX(BIT_COL_CHAR, BIT_COL1_NUM)
#define		BIT_COL2			DEF_PTX(BIT_COL_CHAR, BIT_COL2_NUM)
*/

// registers
#define		BIT_ROW_DDR			DEF_DDR(BIT_ROW_CHAR)
#define		BIT_COL_DDR			DEF_DDR(BIT_COL_CHAR)

#define		BIT_ROW_PORT		DEF_PORT(BIT_ROW_CHAR)
#define		BIT_COL_PORT		DEF_PORT(BIT_COL_CHAR)

// masks
#define		BIT_ROW0_MASK		_BV(BIT_ROW0_NUM)
#define		BIT_ROW1_MASK		_BV(BIT_ROW1_NUM)
#define		BIT_COL0_MASK		_BV(BIT_COL0_NUM)
#define		BIT_COL1_MASK		_BV(BIT_COL1_NUM)
#define		BIT_COL2_MASK		_BV(BIT_COL2_NUM)
#define		BIT_ROW_EN_MASK		_BV(BIT_ROW_EN_NUM)

// pins
#define		BIT_ROW0			DEF_PTX(BIT_ROW_CHAR, BIT_ROW0_NUM)
#define		BIT_ROW1			DEF_PTX(BIT_ROW_CHAR, BIT_ROW1_NUM)
#define		BIT_ROW_EN			DEF_PTX(BIT_ROW_EN_CHAR, BIT_ROW_EN_NUM)


typedef enum {
	keyNoEvent	 = 0,

	keyPage    	 = BIT_ROW0_MASK,

	keyPress     = _BV(KEY_PRESS_BIT),
	keyRelease   = _BV(KEY_RELEASE_BIT),

	keyRowMask    = BIT_ROW0_MASK | BIT_ROW1_MASK,
	keyColumnMask = BIT_COL0_MASK | BIT_COL1_MASK | BIT_COL2_MASK,
} abstract_key_e;

#if ((keyRowMask & keyColumnMask) > 0)
	#error "row and col cannot share the same bit, review your design"
#endif



// a key defines as a set of high bits where one column is pulled low and the setting of the first row
typedef enum {
	// basic keys

	// key-names are derived from mcc315 LCD display keys, but they are only names
	keyMinus =  BIT_COL0_MASK | BIT_COL1_MASK | BIT_ROW1_MASK, // COL2 and ROW0 down
	keyPlus =   BIT_COL0_MASK | BIT_COL2_MASK | BIT_ROW1_MASK, // COL1 and ROW0 down
	keyA = 		BIT_COL1_MASK | BIT_COL2_MASK | BIT_ROW1_MASK, // COL0 and ROW0 down

	keyB =		BIT_COL0_MASK | BIT_COL1_MASK | BIT_ROW0_MASK, // COL2 down, ROW0 up
	keyOK =     BIT_COL0_MASK | BIT_COL2_MASK | BIT_ROW0_MASK, // COL1 down, ROW0 up
	keyEsc =    BIT_COL1_MASK | BIT_COL2_MASK | BIT_ROW0_MASK, // COL0 down, ROW0 up

#ifdef WITH_BITWISE_DOUBLE_KEY
	// possible multiple keys via bit-combine
	keyPlusMinus = BIT_COL0_MASK | BIT_ROW1_MASK,
	keyA_minus   = BIT_COL1_MASK | BIT_ROW1_MASK,
	keyA_plus    = BIT_COL2_MASK | BIT_ROW1_MASK,

	keyOK_B      = BIT_COL0_MASK | BIT_ROW0_MASK,
	keyEsc_B     = BIT_COL1_MASK | BIT_ROW0_MASK,
	keyOK_esc    = BIT_COL2_MASK | BIT_ROW0_MASK,

#endif
	keyAllBits   = BIT_ROW0_MASK  | BIT_ROW1_MASK | keyColumnMask,

} keyMatrix_events_e;


typedef struct {
	uint8_t msk;
	uint8_t page;
	uint8_t pLast[2];		// we have only 2 rows
	timer_uint_t nextPoll;
	uint8_t cnt[NUM_KEYS];	// # columns *  # rows ( * 2 buttons pressed )
} keyMatrix_data_t;


// single and bit-combined keypress map, must be correctly ordered
static const uint8_t keySymbol[NUM_KEYS] = {
		  keyMinus     ,keyPlus    ,keyA
#ifdef WITH_BITWISE_DOUBLE_KEY
		 ,keyPlusMinus ,keyA_minus ,keyA_plus
#endif
		 ,keyB         ,keyOK      ,keyEsc
#ifdef WITH_BITWISE_DOUBLE_KEY
		 ,keyOK_B	   ,keyEsc_B   ,keyOK_esc
#endif
};

// the following keys can be pressed together without interfering since
// they are on different rows(pages)
// those are not one combined event but generate two single events for each key
//  	OK+plus, OK+minus, OK+A
//	    B+plus, B+minus, B+A,
// 		esc+plus, esc+minus, esc+A


// instantiate this in your main program or as global
extern 		keyMatrix_data_t 		keyData;


inline void keymatrix_init()
{
//	keyData.nextPoll.gap = MS_TO_GAP(5UL);

	BIT_ROW_DDR |= (BIT_ROW0_MASK | BIT_ROW1_MASK | BIT_ROW_EN_MASK);

	// is already input after reset
//	BIT_COL_DDR &= ~(BIT_COL0_MASK | BIT_COL1_MASK | BIT_COL2_MASK);

	BIT_ROW_EN = 1;
}


// callback handler, should be called very often in regular intervals
inline uint8_t keyEvent()
{
	if (keyData.msk & keyPage) {
		BIT_ROW0 = 0;
		BIT_ROW1 = 1;
	} else {
		BIT_ROW0 = 1;
		BIT_ROW1 = 0;
	}

//	uDelay150();

	keyData.msk = ((BIT_COL_PORT & keyColumnMask) | (BIT_ROW_PORT & (keyPage|BIT_ROW1_MASK)));
	uint8_t j = (keyData.msk & keyPage) ? (NUM_KEYS/2) : 0;

	uint8_t i, p;

	p = 0;

	for (i=0; i<(NUM_KEYS/2); i++) {
		uint8_t k = i+j;

		if (keyData.msk == keySymbol[k]) {
			if (keyData.cnt[k] < KEY_DEBOUNCE_CNT) {
				keyData.cnt[k]++;
			} else {
				// debounce count reached

				//  reacting on 2 simultaneous independent pressed keys here
				if ((keyData.msk & keyPage) > 0) {
					// page 1 key
					p = 1;
//				} else  {
					// page 0 key
				}
				if (keyData.pLast[p] == (keyData.msk | keyPress)) {
					// hold already pressed key
				} else {
					// changed
					keyData.pLast[p] = keyData.msk | keyPress;
					keyData.page = p;
					return keyData.pLast[p];
				}
				// hold already pressed key
				// return rkm;		// only enter and leave events
			}
		} else {
			if (keyData.cnt[k] > 0) {
				keyData.cnt[k]--;
				if (keyData.cnt[k] < (KEY_DEBOUNCE_CNT/2)) {
					if ((keyData.msk & keyPage) > 0) {
						// page 1 key
						p = 1;
//					} else  {
						// page 0 key
					}
					if ((keyData.pLast[p] & keyPress) > 0) {
						keyData.pLast[p] &= ~keyPress;
						keyData.pLast[p] |= keyRelease;
						keyData.page = p;
						return keyData.pLast[p] ;
					}
				}
			}
		}
	}
	return keyNoEvent;
}




#endif /* KEYMATRIX_H_ */
