/*
 * S19File.h
 *
 *  Created on: 11.06.2020
 *      Author: pantec
 */

#ifndef S19FILE_H_
#define S19FILE_H_

#include "S19rec_c.h"
#include "hc908_variants.h"

class S19KeyFile
{
	uint8_t _keyBytes[16];
	bool _keyIsValid[16];

public:
	uint8_t nKeys;
	uint16_t firstAddr;

	S19KeyFile();
	virtual ~S19KeyFile();

	void _clear();

	bool Init(mc86hc908_variants_e type, char *filename);

	uint8_t *getKeybytes();
};

#endif /* S19FILE_H_ */
