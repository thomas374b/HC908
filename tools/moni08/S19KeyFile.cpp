/*
 * S19File.cpp
 *
 *  Created on: 11.06.2020
 *      Author: pantec
 */

#include "S19KeyFile.h"

#include "tns_util/readln.h"

#include <stdio.h>
#include <string.h>
#include <errno.h>


void S19KeyFile::_clear()
{
	memset(_keyBytes, 0, N_MAX_SECURITY_BYTES);
	memset(_keyIsValid, 0, N_MAX_SECURITY_BYTES);

	nKeys = 0;
	firstAddr = 0;
}

S19KeyFile::S19KeyFile()
{
	_clear();
}

S19KeyFile::~S19KeyFile()
{
}


bool S19KeyFile::Init(mc86hc908_variants_e cpuIdx, char *s19FileName)
{
	if ((cpuIdx >= mc68hc908_undefined) || (cpuIdx < 0)) {
		fprintf(stderr,"unimplemented CPU variant %d\n", cpuIdx);
#ifdef WITH_DEBUG_
		if (verbose_mode) {
			// print help
			for (uint8_t i=0; i<sizeof(cpu_firmware_addr)/sizeof(cpu_firmware_addr_t); i++) {
				fprintf(stderr," -> %d  %s  RAM:%d  FLASH:%d\n"
						,cpu_firmware_addr[i].idx
						,cpu_firmware_addr[i].variantStr
						,cpu_firmware_addr[i].ramSize
						,cpu_firmware_addr[i].flashSize
						);
			}
		}
#endif
		return false;
	}

	const security_byte_addrs_t *sec_byte_addr = &cpu_firmware_addr[cpuIdx].secBytes;

	t_buffer FBuf;

	if (FBuf.Init(s19FileName, 128*1024) == false) {
		fprintf(stderr,"open %s failed, %s\n", s19FileName, strerror(errno));
		return false;
	}

	_clear();

	firstAddr = sec_byte_addr->addr[0];

	int lineCnt = 1;
	char *L = FBuf.ReadLn();
	while (L != NULL) {
		t_s19Line S1Line,*got;

		uint8_t lineCheck = 0;
		got = C_S19rec::Filled(L, &S1Line, &lineCheck);

		if (got != NULL) {
#ifdef WITH_DEBUG_
			fprintf(stderr,"got.addr:%ld, got.buffer:%ld, got.Len:%ld\n",got->Addr, (long)&got->buffer[0], got->Len);
#endif
			switch(got->ID) {
				case '0':	// TODO: handle header info
					break;

				case '1':
				case '2':
				case '3':
					if ((lineCheck == 0xFF) && (got->ID)) {
						// parse
						for (uint8_t i=0; i<got->payloadLen; i++) {
							uint16_t iAddr = i + got->Addr;

							for (uint8_t j=0; j<sec_byte_addr->nAddr; j++) {
								if (iAddr == sec_byte_addr->addr[j]) {  // matched an address
									if (!_keyIsValid[j]) { // that we dont have yet
										_keyBytes[j] = got->buffer[i];
										_keyIsValid[j] = true;	// take the first
									}
								}
							}
						}
#ifdef WITH_DEBUG_
					} else {
						fprintf(stderr,"no checksum match [0x%02X]\n", lineCheck);
#endif
					}
					break;

				default:
#ifdef WITH_DEBUG_
					if (verbose_mode) {
						fprintf(stderr,"record with ID [%c] ignored\n", got->ID);
					}
#endif
					break;
			}
		} else {
#ifdef WITH_DEBUG_
		    if (verbose_mode) {
				fprintf(stderr,"skipping invalid S19line [%s]\n", L);
		    }
#endif
		}
		L = FBuf.ReadLn();
	}
	FBuf.Done();

	// check if we got all we need
	nKeys = sec_byte_addr->nAddr;
	for (uint8_t j=0; j<sec_byte_addr->nAddr; j++) {
		if (!_keyIsValid[j]) {
			nKeys = 0;
			break;
		}
	}
	return (nKeys > 0);
}

uint8_t *S19KeyFile::getKeybytes()
{
	if (nKeys < 1) {
		return NULL;
	}
	return &_keyBytes[0];
}
