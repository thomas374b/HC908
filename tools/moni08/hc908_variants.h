/*
 * hc908_variants.h
 *
 *  Created on: 11.06.2020
 *      Author: pantec
 */

#ifndef HC908_VARIANTS_H_
#define HC908_VARIANTS_H_


#include <stdint.h>

// implemented
#define	N_HC908_VARIANTS		4

typedef enum {
	// indices into cpu_firmware_addr table, keep them in order
	mc68hc908jk1 = 0,
	mc68hc908jk3 = 1,
	mc68hc908jb8 = 2,
	mc68hc908mr32 = 3,		// TODO: implement this CPU

	mc68hc908_undefined = N_HC908_VARIANTS,
} mc86hc908_variants_e;

#define	N_MAX_SECURITY_BYTES		16

typedef struct {
	uint8_t nAddr;
	uint16_t addr[N_MAX_SECURITY_BYTES];
} security_byte_addrs_t;


typedef struct {
	mc86hc908_variants_e idx;

	uint16_t CtrlByte;
	uint16_t cpuSpeed;
	uint16_t Laddr;
	uint16_t dataBuf;
	uint16_t dataSize;

	uint16_t flashStart;
	uint16_t flashSize;
	uint16_t vectorStart;
	uint16_t FLBPR;

	uint16_t erase;
	uint16_t program;

	uint16_t HX;
	uint16_t jumpAddr;

	uint16_t ramStart;
	uint16_t ramSize;
	security_byte_addrs_t secBytes;
	char variantStr[32];
} cpu_firmware_addr_t;


extern const cpu_firmware_addr_t cpu_firmware_addr[N_HC908_VARIANTS] ;


#endif /* HC908_VARIANTS_H_ */
