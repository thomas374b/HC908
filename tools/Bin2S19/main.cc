

#define MAIN_G
#include "tns_util/copyright.h"

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

#include "tns_util/readln.h"
#include "tns_util/mkopts.h"



#define DEBUG	1

#include "S19rec_c.h"


C_S19rec 	*S19Rec;


/*
void check_hexnum_base(int i, double f, char *fn);
void check_hexnum_jump(int i, double f, char *fn);
*/

t_opts opts[] = {
 	{empty_func, e_hexnum, 'b',"base\0","offset address\t\t\0",false,0,0.0,"0x0\0"},
#define BASE_ADDR opts[0].i

	{empty_func,e_string,'f',"file\0","binary input file\t\0",false,0,0.0,"xyz.bin\0                                                                                                                "},
#define filename opts[1].s

	{empty_func,e_boolean,'s',"s19\0","make s19 output\t\t\0",false,0,0.0,"xyz.bin\0                                                                                                                "},
#define s19out opts[2].b

	{empty_func,e_boolean,'r',"ras\0","random access segments\t\0",false,0,0.0,"xyz.bin\0                                                                                                                "},
#define rasin opts[3].b

	{empty_func,e_boolean,'w',"raw\0","make binary output\t\0",false,0,0.0,"xyz.bin\0                                                                                                                "},
#define rawout opts[4].b

	{empty_func,e_integer,'c',"record\0","s19 record lenght\t\0",false,16,0.0,"xyz.bin\0                                                                                                                "},
#define RecSize opts[5].i

	{empty_func,e_boolean,'x',"hexout\0","make raw hex dump\t\0",false,0,0.0,"xyz.bin\0                                                                                                                "},
#define hexout opts[6].b

	{empty_func,e_integer,'n',"addr\0","number of address bytes\t\0",false,2,0.0,"\0"},
#define ADDR_LEN	opts[7].i	

	{empty_func, e_hexnum,'j',"jump\0","addr to run program\t\0", false, 0xFFFE, 0.0,"0xFFFE\0"},
#define JUMP_START	opts[8].i

	{empty_func,e_boolean,'i',"ignore\0","checksum from buggy s19 file\0",false,0,0.0,"xyz.bin\0                                                                                                                "},
#define IGNORE_CHECKSUM opts[9].b

	{empty_func,e_integer,'z',"size\0","flashable area before vects\0",false,4096,0.0,"xyz.bin\0                                                                                                                "},
#define MAX_FLASH_SIZE opts[10].i
#define VECTOR_MODE opts[10].b


/*
 	{do_prefix,e_boolean,'b',"begin\0","match only trigrams at start of string\0",false,0,0.0,"\0"},
#define prefix_mode opts[0].b
 	{do_postfix,e_boolean,'e',"end\0","match only trigrams at end of string\0",false,0,0.0,"\0"},
#define postfix_mode opts[1].b
 	{do_invert,e_boolean,'i',"invert\0","print the matching strings\t\0",false,0,0.0,"\0"},
#define invert_mode opts[2].b
	{empty_func,e_string,'d',"database\0","use word list to extract trigrams\0",false,0,0.0,"database.txt\0                                                                                                          "},
#define database opts[4].s
	{check_penta,e_integer,'p',"penta\0","check for pentagrams or whatever\0",false,3,0.0,"\0                                                                                                                "},
#define tree_depth opts[6].i
	{empty_func,e_boolean,'g',"generate\0","list all trigrams in namespace\t\0",false,3,0.0,"\0                                                                                                                "},
#define generate opts[7].b
*/

	{empty_func,e_unknown,'\0',"\0","\0",0,0,0.0,"\0"}
};



// #define S1_ID	1


/*




void check_hexnum_base(int i, double f, char *fn)
{
	check_hexnum(i, fn, &BASE_ADDR);
}

void check_hexnum_jump(int i, double f, char *fn)
{
	check_hexnum(i, fn, &JUMP_START);
}
*/


void makeRaw(void) 
{
	t_buffer FBuf;
	char fn[256+16];
	sprintf(fn,"%s.s19",filename);

	if (verbose_mode) {
		fprintf(stderr,"open s19 format file \"%s\"\n",filename);
	}	
	if (FBuf.Init(fn,32768) == false) {
		fprintf(stderr,"open %s.s19 failed\n",filename);
		_exit(1);
	}
	char Binary[0x10000];
	memset(Binary,0xff,0x10000);
	uint32_t lowest,highest;
	lowest = 0x10000;
	highest = 0;
	
	char *L = FBuf.ReadLn();
	while (L != NULL) {
		t_s19Line S1Line,*got;
				
/*		int l = strlen(L);
		if (L > 0) {
		    if (L[l-1] == 13) {
			if (verbose_mode) {
			    fprintf(stderr,"DOS linefeed removed\n");
			}     
			L[l-1] = 0;	
		    }
		}
*/
#ifdef WITH_DEBUG_
		if (verbose_mode) {
		    fprintf(stderr,"parsing line \"%s\"\n",L);
		}
#endif
		uint8_t lineCheck = 0;
		got = C_S19rec::Filled(L, &S1Line, &lineCheck);

		if (got != NULL) {
#ifdef WITH_DEBUG_
			fprintf(stderr,"got.addr:%ld, got.buffer:%ld, got.Len:%ld\n",got->Addr, (long)&got->buffer[0], got->Len);
#endif
			switch(got->ID) {
				case '1':
				case '2':
				case '3':
					if ((lineCheck == 0xFF) && (got->ID)) {
						memcpy(&Binary[got->Addr], got->buffer, got->payloadLen);

						if (highest < got->Addr+got->payloadLen) {
							highest = got->Addr+got->payloadLen;
						}
						if (lowest > got->Addr) {
							lowest = got->Addr;
						}
					} else {
						fprintf(stderr,"no checksum match [0x%02X]\n", lineCheck);
					}
					break;

				default:
					fprintf(stderr,"record with ID [%c] ignored\n", got->ID);
					break;
			}
		} else {
//#ifdef DEBUG
		    if (verbose_mode) {
				fprintf(stderr,"no valid S19line [%s]\n",L);
		    }
//#endif
		}
		L = FBuf.ReadLn();
	}
	FBuf.Done();
	
	int Size = (highest - lowest);
	if (Size > 0) {
		char fn[256+16];
		sprintf(fn,"%s-0x%04X.bin",filename,lowest);
		int fd = open(fn,O_RDWR|O_CREAT|O_TRUNC,0644);
		if (fd > 0) {
			if (write(fd,&Binary[lowest],Size) != Size) {
				fprintf(stderr,"could not write \"%s\": %s\n",fn,strerror(errno));
			}
			close(fd);
		}
	} else {
	    if (verbose_mode) {
			fprintf(stderr,"size was 0, no file written\n");
	    }
	}
}

int main(int argc, char *argv[])
{
	scan_args(argc,argv,opts);

	S19Rec = new C_S19rec( RecSize, (s19_addrLen_e)ADDR_LEN );

	if (s19out || rasin) {
		if (verbose_mode) {
			fprintf(stderr,"open binary file \"%s\"\n",filename);
		}
		S19Rec->makeS19(filename, BASE_ADDR, rasin, hexout, JUMP_START, VECTOR_MODE?MAX_FLASH_SIZE:-1);
	} else {
		if (rawout) {
			if (verbose_mode) {
			    fprintf(stderr,"make raw output from s19\n");
			}        
			makeRaw();
		} else {
			fprintf(stderr,"you have to choose option -s or -r\n");
		}
	}
}
