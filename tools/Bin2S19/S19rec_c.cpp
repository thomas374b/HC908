/*
 * S19rec_c.cpp
 *
 *  Created on: 04.06.2020
 *      Author: pantec
 */

#include "S19rec_c.h"

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

C_S19rec::C_S19rec(int rec_size, s19_addrLen_e addr_len)
{
	ChkSum = new S19ChkSum(addr_len);
	bytesPerLine = rec_size;
}

C_S19rec::~C_S19rec()
{
	delete ChkSum;
}

void S19ChkSum::Init(uint8_t bytesPerLine, uint32_t addr)
{
	sum = 0;
	sum += bytesPerLine;	// # of bytes per line (payload)
	sum += addrBits;		// # of address-bytes
	sum += 1; 				// checksum

	uint8_t shift = 0;
	for (int i=0; i<addrBits; i++) {
		sum += ((addr >> shift) & 0xFF);
		shift += 8;
	}
}

S19ChkSum::S19ChkSum(s19_addrLen_e ab/*, uint8_t bpl*/)
{
	addrBits = ab;
}

void S19ChkSum::add(uint8_t B)
{
	sum += B;
}

uint8_t S19ChkSum::final()
{
	return (uint8_t)(0xff - (sum & 0xff));
}

char *S19ChkSum::LineHeadRaw(int index, uint32_t addr, char *buffer, int bpl)
{
	Init(bpl, addr);

	char sformat[64];

	memset(sformat, 0, 64);
	memset(buffer, 0, 64);

	sprintf(sformat,"S%d%%02X%%0%dX", index, addrBits *2);

	sprintf(buffer, sformat, bpl +addrBits +1, addr);

	return buffer;
}


char *S19ChkSum::Append(uint8_t byte, char *buffer)
{
	char FC[16];

	add(byte);
	sprintf(FC,"%02X", byte );
	strcat(buffer, FC);

	return buffer;
}

char *S19ChkSum::Preambel(char *modulName, uint8_t version, uint8_t revision, char *comment, char *buffer)
{
	int pll = 12;
	int cmtLen = 0;

	if (comment != NULL) {
		cmtLen = strlen(comment);
		pll += cmtLen;
	}

	LineHeadRaw(0, 0, buffer, pll);

	int mnl = 0;
	if (modulName != NULL) {
		mnl = strlen(modulName);
	}

	for (int i=0; i<10; i++) {
		uint8_t byte = ' ';	// pad to end with spaces
		if (i < mnl) {
			byte = modulName[i];
		}
		Append(byte, buffer);
	}
	Append(version, buffer);
	Append(revision, buffer);

	if (cmtLen > 0) {
		for (int i=0; i<cmtLen; i++) {
			Append(comment[i], buffer);
		}
	}

	return Trailer(buffer);
}

char *S19ChkSum::DataRowHead(int bpl, uint32_t addr, char *buffer)
{
	return LineHeadRaw(addrBits-1, addr, buffer, bpl);
}

char *S19ChkSum::Trailer(char *buffer)
{
	char FC[16];
	sprintf(FC,"%02X", final() );
	strcat(buffer, FC);
	return buffer;
}

char *S19ChkSum::Counter(uint32_t n, char *buffer)
{
	LineHeadRaw(5, n, buffer, 0);
	return Trailer(buffer);
}


char *S19ChkSum::EndBlock(uint32_t addr, char *buffer)
{
	LineHeadRaw(11-addrBits, addr, buffer, 0);
	return Trailer(buffer);
}


char *S19ChkSum::ArrayToS19(uint16_t addr, uint8_t *data, uint8_t len, char *buffer)
{
	if (buffer == NULL) {
		return NULL;
	}
	DataRowHead(len, addr, buffer);
	for (uint8_t i=0; i<len; i++) {
		Append(data[i], buffer);
	}
	return Trailer(buffer);
}



unsigned char C_S19rec::Hex2Num(char C) {
	if ((C >= '0') && (C <= '9')) {
		return (C - '0');
	}
	if ((C >= 'A') && (C <= 'F')) {
		return (C - 'A')+10;
	}
	if ((C >= 'a') && (C <= 'f')) {
		return (C - 'a')+10;
	}
	return 0;
}

unsigned char C_S19rec::Hex2Byte(char *S)
{
	return (Hex2Num(S[0]) << 4) | Hex2Num(S[1]);
}



void C_S19rec::makeS19(char *filename, int baseaddr, bool rasin, bool hexout, int jumpStart, int max_size)
{
	unsigned char buf[65536 + 4096];

	int ifd = open(filename,O_RDONLY);
	if (ifd < 0) {
		fprintf(stderr,"open(\"%s\") failed: %s\n",filename,strerror(errno));
		_exit(1);
	}
	int Size = read(ifd,buf,65536);
	close(ifd);

	int k = 0;
	int Length = 0;
	int q = 0;
	int Len = 0;
	int nRec = 0;

	bool vector_mode = false;
	bool vector_mode_requested = (max_size > 0);

	char buffer[256];
	printf("%s\r\n", ChkSum->Preambel(filename, 1, 2, NULL, buffer) );

	char appendBuf[256];
	char outputLine[256];
	bool isAllFF = false;

	int objSize = 0;

	while (k < Size) {
		memset(outputLine,0,256);
		memset(appendBuf,0,256);


		if (rasin) {
			if (q == Length) {
				q = 0;
			}
			if (q == 0) {
				unsigned short *O,*L;
				O = (unsigned short *)&buf[k];
				L = (unsigned short *)&buf[k+2];
				baseaddr = *O;
				Length = *L;
				k += 4;
/*
				if (verbose_mode) {
					fprintf(stderr,"segment: 0x%04x, len: %d\n",*O,*L);
				}
*/
			}
			Len = Length -q;
		} else {
			Len = Size -k;
		}

		if (!vector_mode) {
			if (vector_mode_requested) {
				if (k > max_size) { // enter vector mode
					vector_mode = true;
					bytesPerLine = 2;
				}
			}
//		} else {
//			Len = 2;
		}

		if (isAllFF) {
			// previous line was skipped
			// find next not FF
			for (int i=0; i<Size; i++) {
				int j = i+k;
				if (j >= Size) {
					break;
				}
				if (buf[j] != 0xFF) {
					isAllFF = false;

					// adjust Len
					Len = Size -k;

//					if ((vector_mode) && (Len < 64)) {
//						bytesPerLine = 64;
//					}
					break;
				}
			}
		}

		if (Len > bytesPerLine) {
			Len = bytesPerLine;
		}

		int Addr = baseaddr;

		if (rasin) {
			Addr += q;
		} else {
			Addr += k;
		}

		if (Addr > (1 << (ChkSum->addrBits*8))) {
			fflush(stdout);
			fprintf(stderr,"address 0x%08X too large, exiting\n", Addr);
			_exit(2);
		}
//		fprintf(stderr,"Addr: 0x%08X  base: 0x%08X q: %d  k: %d\n", Addr, baseaddr, q, k);

		isAllFF = true;



		if (!(hexout)) {
			sprintf(appendBuf, "%s", ChkSum->DataRowHead(Len, Addr, buffer) );
		} else {
			sprintf(appendBuf, "%04X:",Addr);
		}
		strcat(outputLine,appendBuf);

		for (int i=0; i<Len; i++) {
			if (buf[i+k] != 0xFF) {
				isAllFF = false;
			}
			sprintf(appendBuf, "%02X", buf[i+k]);
			strcat(outputLine, appendBuf);

			ChkSum->add(buf[i+k]);
		}

		if (!(hexout)) {
			sprintf(appendBuf, "%02X\r\n", ChkSum->final() );
		} else {
			sprintf(appendBuf, "\n");
		}
		strcat(outputLine,appendBuf);

		bool skipThis = false;

		if (!(hexout)) {
			if (isAllFF) {
				skipThis = true;
			}
		}
		if (!skipThis) {
			objSize += Len;
			printf("%s", outputLine);
		}

		k += Len;
		q += Len;
		nRec++;
	}
	if (!(hexout)) {
		printf("%s\r\n", ChkSum->Counter(nRec, buffer));
		printf("%s\r\n", ChkSum->EndBlock(jumpStart, buffer));
	}
	fprintf(stderr,"objsize: %d bytes\n", objSize);
}


void C_S19rec::chomp(char *L)
{
	if (L == NULL) {
		return;
	}
	if (*L == '\0') {
		return;
	}

	int n = strlen(L)-1;

	while ((L[n] == '\n') || (L[n] == '\r')) {
		L[n] = '\0';
		n--;
	}
}


t_s19Line *C_S19rec::Filled(char *Line, t_s19Line *tmpLine, uint8_t *chkSum)
{
	if ((Line == NULL) || (tmpLine == NULL)) {
		fprintf(stderr,"%s:%s() Line %08lX or tmpLine %08lX is NULL\n",__FILE__, __func__, (long)Line, (long)tmpLine);
		return NULL;
	}

	int Len = strlen(Line);
	if (Len <= 0) {
		fprintf(stderr,"%s::%s  Len is <= 0\n",__FILE__, __func__);
		return NULL;
	}

//	unsigned int CheckSum = 0;

	int i = 0;
	int k = 0;
	if ((Len % 2) == 1) {
#ifdef WITH_DEBUG_
//	    if (verbose_mode) {
	    	fprintf(stderr,"Length %d is odd, removing DOS linefeed\n", Len);
//	    }
#endif
	    Len--;
	}
	int addrBits = 2;

	tmpLine->Addr = 0;
	tmpLine->ID = 0;
	tmpLine->payloadLen = 0;
	memset(tmpLine->buffer, 0, 256);

	uint8_t lastByte = 0;
	uint8_t secondLastByte = 0;

	while (i < Len) {
		char *Test = Line + i;
#ifdef DEBUG
/*
		if (verbose_mode) {
		    fprintf(stderr,"evaluating char[%c](byte 0x%04x) at pos %d, checksum 0x%08x\n",*Test,Hex2Byte(Test),i,CheckSum);
		}
*/
#endif

		switch(i) {
			case 0: // ID
				if (*Test != 'S') {
					fprintf(stderr,"missing S-Record\n");
					return NULL;
				}
				switch(Test[1]) {
					case '1':
					case '2':
					case '3':
						tmpLine->ID = Test[1];
						addrBits = Test[1]+2-'1';
						break;

					case '0': 	// Preambel
					case '5':	// line counter
					case '9':	// End Blocks
					case '8':
					case '7':
						tmpLine->ID = Test[1];
						// TODO: handle header S0
						// TODO: handle record counter S5
						// TODO: handle EndBlocks
						break;

					default:
						fprintf(stderr, "undefined S-Record [%c]\n", Test[1]);
						return NULL;
				}
				break;

			case 2: // Len
				tmpLine->payloadLen = Hex2Byte(Test) -1 -addrBits;
				*chkSum += addrBits;
				*chkSum += tmpLine->payloadLen;
				*chkSum += 1;  // checksum itself
				break;
#if 0
			case 4: // Addr
				tmpLine->Addr = Hex2Byte(Test);
				CheckSum += tmpLine->Addr;
				tmpLine->Addr <<= 8;

				tmpLine->Addr |= (Hex2Byte(&Test[2]));
				CheckSum += (tmpLine->Addr & 0xff);
				i+=2;
				break;
#endif

			default: // Bytes
				if (i < (4 + addrBits)) {
					uint8_t byte = Hex2Byte(Test);
					*chkSum += byte;

					tmpLine->Addr <<= 8;
					tmpLine->Addr |= byte;

					lastByte = byte = Hex2Byte(&Test[2]);
					*chkSum += byte;

					tmpLine->Addr <<= 8;
					tmpLine->Addr |= byte;
#ifdef WUTH_DEBUG_
					fprintf(stderr,"	i:%d, b:%02x, a:%X, ab:%d\n", i, byte, tmpLine->Addr, addrBits);
#endif
					i += 2;
				} else {
					lastByte = tmpLine->buffer[k] = Hex2Byte(Test);
					*chkSum += tmpLine->buffer[k];
					k++;
				}
				break;
		}
#ifdef WUTH_DEBUG_
		fprintf(stderr,  "i:%d k:%d, l:%d chksum:x%X lb:%X\n", i, k, tmpLine->payloadLen, CheckSum, lastByte);
#endif
		i+=2;
	}
//	CheckSum -= lastByte;

#ifdef WUTH_DEBUG_
    fprintf(stderr,"checksum 0x%04x matched? lB:%02X\n", *chkSum, lastByte);
#endif
//	if ((CheckSum & 0xff) != 0xff) {
//#ifdef DEBUG
/*
		if (verbose_mode) {
		    fprintf(stderr,"checksum 0x%04x did not match\n",CheckSum);
//		    exit(1);
		}
*/
//#endif
//		return NULL;
//	}

	return tmpLine;
}

