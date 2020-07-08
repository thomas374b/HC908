/*
 * S19rec_c.h
 *
 *  Created on: 04.06.2020
 *      Author: pantec
 */

#ifndef S19REC_C_H_
#define S19REC_C_H_

#include <inttypes.h>

typedef struct {
	unsigned char ID;
	unsigned char payloadLen;		// len of payload
	uint32_t Addr;
	unsigned char buffer[256];
} t_s19Line;

typedef struct {
    unsigned char module_name[20];
    unsigned short version;
    unsigned short revision;
} s19header_t;

typedef enum {
	s19_16bitAddr = 2,
	s19_24bitAddr = 3,
	s19_32bitAddr = 4,
} s19_addrLen_e;

class S19ChkSum {
	int sum;
public:
	s19_addrLen_e addrBits;		// {2,3,4}   aka 16-,24-,32-bit address

	S19ChkSum(s19_addrLen_e ab);

	void Init(uint8_t bytesPerLine, uint32_t addr);

	void add(uint8_t B);
	uint8_t final();

	char *Preambel(char *modulName, uint8_t version, uint8_t revision, char *comment, char *buffer);
	char *DataRowHead(int bpl, uint32_t addr, char *buffer);
	char *EndBlock(uint32_t addr, char *buffer);
	char *Counter(uint32_t n, char *buffer);

	char *Append(uint8_t byte, char *buffer);
	char *Trailer(char *buffer);
	char *ArrayToS19(uint16_t addr, uint8_t *data, uint8_t len, char *buffer);
protected:
	char *LineHeadRaw(int index, uint32_t addr, char *buffer, int bpl);
};


class C_S19rec {
	S19ChkSum *ChkSum;
	uint8_t bytesPerLine;
public:
	static unsigned char Hex2Num(char C);
	static unsigned char Hex2Byte(char *S);
	static void chomp(char *L);

	C_S19rec(int rec_size, s19_addrLen_e addr_len);
	virtual ~C_S19rec();

	static t_s19Line *Filled(char *Line, t_s19Line *tmpLine, uint8_t *chkSum);
	void makeS19(char *filename, int baseaddr, bool rasin, bool hexout, int jumpStart, int max_size);
};

#endif /* S19REC_C_H_ */
