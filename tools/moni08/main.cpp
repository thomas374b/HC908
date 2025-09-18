

#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

#define MAIN_G
#include "tns_util/copyright.h"

#include "linux_serial.h"
#include "S19rec_c.h"
#include "S19KeyFile.h"
#include "hc908_monitor_commands.h"


#include "tns_util/utils.h"
#include "tns_util/mkopts.h"
#include "tns_util/timer.h"
#include "tns_util/readln.h"



typedef struct {
	hc908_monitor_cmds_e cmd;
	unsigned char nSend;
	unsigned char nReceive;
} hc908_monitor_cmd_t;

static hc908_monitor_cmd_t hc908_monitor_cmd[] = {
		 {eMonitorReadMem, 3, 1}
		,{eMonitorWriteMem, 4, 0}
		,{eMonitorIncRead, 1, 2}
		,{eMonitorIncWrite, 2, 0}
		,{eMonitorReadSP, 1, 2}
		,{eMonitorExec, 1, 0}

		,{eMonitorSecurity, 8, 0}

		,{eMonitorUninitialized,0,0}
};

typedef enum {
	eReadMem = 0,
	eWriteMem = 1,
	eIncRead = 2,
	eIncWrite = 3,
	eReadSP = 4,
	eExec =  5,

	eSendKeyBytes = 6,

	erUninitialized = 7,
} hc908_monitor_indices_e;


typedef struct {
	unsigned char addrHigh;
	unsigned char addrLow;
	unsigned char value;
} write_mem_t;

/*
typedef struct {
	unsigned char addrHigh;
	unsigned char addrLow;
	unsigned char value;
} write_mem_t;
*/
// 1:7
// 2:6
// 3:5
// 4:4
// 5:3
// 6:2
// 7:1

// static uint8_t security_key[32];

int SFD = -1;
int rtsbit = -1;
int dtrbit = -1;
bool lateConfigure = false;

static uint8_t _SKEY_BYTES[N_MAX_SECURITY_BYTES];

#define 	WITH_DEBUG		1

extern void open_function(int i, double f, char *s);
extern void stop_function(int i, double f, char *s);
extern void parity_function(int i, double f, char *s);
extern void baud_function(int i, double f, char *s);
extern void toggle_function(int i, double f, char *s);
extern void gap_delay_function(int i, double f, char *s);

extern void send_monitor_function(int i, double f, char *s);
extern void read_monitor_function(int i, double f, char *s);
extern void send_function(int i, double f, char *s);
extern void prefix_function(int i, double f, char *s);
extern void exec_function(int i, double f, char *s);
extern void SP_function(int i, double f, char *s);
extern void waitBreak_function(int i, double f, char *s);
extern void readVar_function(int i, double f, char *s);
extern void flashProg_function(int i, double f, char *s);
extern void tty_function(int i, double f, char *s);
extern void reset_function(int i, double f, char *s);
extern void erase_function(int i, double f, char *s);
extern void jump_function(int i, double f, char *s);
extern void por_function(int i, double f, char *s);

extern void cpu_function(int i, double f, char *s);
extern void writeVar_function(int i, double f, char *s);

void exit_proc(int result, bool restore=true)
{
	if (verbose_mode) {
		fprintf(stderr,"controlled exit\n");
	}
	if (restore) {
		if (restore_and_close(SFD) != 0) {
			if (verbose_mode) {
				fprintf(stderr,"serial term flags not restored\n");
			}
		}
	} else {
		if (verbose_mode) {
			fprintf(stderr,"serial term flags not restored\n");
		}
	}
	exit(result);
}


#if 0
extern void crtscts_function(int i, double f, char *s);
extern void xonxoff_function(int i, double f, char *s);
#endif


bool needBootLoader = false;

t_opts opts[] = {
	{open_function, e_string, 'd',"device","serial port\t\t\0",false,0,0.0,"/dev/ttyS0\0"},
#define PORT opts[0].s

	{baud_function, e_integer,'b',"baud","rate to select\t\t\0",false,9600,0.0,"\0                                                                                                          "},
#define BAUD opts[1].i

	{empty_func,e_integer,'e',"extra","chars to extend delays\t\0",false, 1, 0.0, ""},
#define	GAP_EXTRA	opts[2].i

	{prefix_function, e_string,'k',"key","send string before download\0",false,0xff,0.0,"KEYBYTES"},
#define bSendKeybytes	opts[3].b
#define SKEY_BYTE_ARG	opts[3].s

	{toggle_function, e_integer,'t',"toggle","dtr or rts wire\t\t\0",false,0,0.0,""},
#define bShake opts[4].b
#define TOGGLE_BITS opts[4].i

	{empty_func,e_integer,'#',"hash","mark printing on\t\0",false,31,0.0,"\0                                                                                                          "},
#define bHashPrinting opts[5].b
#define bHashPrintModulo opts[5].i

	{gap_delay_function,e_integer,'g',"gap","wait ms before continuing\0",false, 800, 0.0,""},
#define bTimeout opts[6].b
#define GAP_TIMEOUT opts[6].i

	{empty_func,e_integer,'a',"addr","base addres to start read/write\0",false,0,0.0,"\0                                                                                                          "},
#define RAM_OFFS opts[7].i

	{empty_func,e_integer,'n',"need","bytes to read/write at least\0",false,128, 0.0, ""},
#define iNeedbytes opts[8].i
#define bNeedbytes opts[8].b

	{waitBreak_function, e_integer,'w',"wait","ms until break received or exit\0",false, 30, 0.0, ""},
#define	BREAK_WAIT_MSEC	opts[9].i	// in milliseconds

	{readVar_function, e_integer,'m',"value","read from memory location\0",false, 0x80, 0.0, ""},
#define	READ_VAR_ADDR	opts[10].i	// in milliseconds

	{empty_func,e_integer,'M',"MHz","of system clock in target\0",false, 2, 0.0, ""},
#define CPU_MHZ			opts[11].i

	{cpu_function,e_string,'c',"cpu","variant-name in target\t\0",false, 1, 0.0, "hc908jb8"},
#define	CPU_VARIANT_NAME	opts[12].s

	{reset_function, e_boolean,'R',"reset","device and write bootloader\0",false,128, 0.0, ""},
// #define GOT_RESET_ARG		opts[13].b

	{jump_function ,e_integer,'j',"jump","execute code in target\t\0",false, 0xEC00, 0.0, ""},
#define JUMP_ADDR		opts[14].i

	{reset_function, e_integer,'P',"por","power-on-reset with wait\0",false,5000, 0.0, ""},
#define POR_RESET_ARG		opts[15].i
#define IS_POR_RESET		opts[15].b

	{empty_func,e_integer,'B',"backup","file creation mask {1,2,4}\0",false, 3, 0.0, ""},
#define BACKUP_FC_MASK		opts[16].i

	{cpu_function,e_integer,'C',"CPU","variant-index in target\t\0",false, 2, 0.0, ""},
#define CPU_VARIANT		opts[17].i

	{erase_function, e_boolean,'Z',"zap","mass erase flash memory\t\0",false,128, 0.0, ""},
#define	IS_ERASE_ACTION		opts[18].b

	{writeVar_function, e_integer,'p',"poke","write to memory location\0",false, 0x80, 0.0, ""},
//#define	WRITE_VAR_ADDR	opts[19].i	// in milliseconds

	{flashProg_function, e_string, 'f',"flash","s19 file to target eeprom\0",false, 0, 0.0, "target.s19\0"},

	{tty_function, e_boolean,'T',"tty","serial terminal IO with device\0",false,128, 0.0, ""},

	{send_monitor_function, e_string, 'l',"load","file(name) to RAM\t\0",false, 0, 0.0, "eeprburn.bin\0"},
	{read_monitor_function, e_string, 'r',"read","monitor data to file(name)\0",false, 0, 0.0, "target-backup.bin\0"},

	{exec_function, e_boolean,'e',"exec","fetch vector from SP and run\0",false,0,0.0,""},
	{SP_function, e_boolean,'s',"stack","pointer read\t\t\0",false,0,0.0,""},



// TODO:
//	{send_function, e_string, 's',"send","data to tty\t\t\0",false, 0, 0.0, "target.bin\0"},


#if 0
	{baud_function, e_integer,'i',"size","of serial data word\t\0",false,8,0.0,""},
#define iSerialBits opts[2].i

	{stop_function, e_boolean,'p',"stop","use extra stop bit\t\0",false,0,0.0,""},
#define iSerialStop opts[3].i

	{parity_function,e_string,'e',"parity","settings\t\t\0",false,'n',0.0,"n"},
#define iSerialParity opts[4].i


	{buffer_delay_func, e_integer, 'L',"len","# of chars to calculate delay\0",false, 32, 0.0,""},
#define iBlockRecv opts[7].i
#define bBlockRecv opts[7].b

	{empty_func,e_integer,'f',"prefix","send byte before download\0",false,0xff,0.0,""},
#define bSendFF opts[2].b
#define iPrefix opts[2].i



	{empty_func,e_integer,'o',"dosLF","send ^M before ^J\t\0",false, 0, 0.0,"\0                                                                                                          "},
#define bDosLFMode opts[9].b
#define iDosLFMode opts[9].i

	{empty_func,e_boolean,'u',"number","start with number display mode\0",false,1200,0.0,"\0                                                                                                          "},
#define bNumberMode opts[10].b

	{empty_func,e_integer,'r',"retries","to fetch a downloaded byte back\0",false,255,0.0,""},
#define maxHsRetries opts[11].i

	{empty_func,e_integer,'w',"width","of terminal\t\t\0",false,128,0.0,""},
#define iLineLength opts[12].i


	{xonxoff_function,e_boolean,'y',"xonxoff","use software flow control\0",false,0,0.0,"n"},
#define bSerialXonXoff opts[16].b


	{empty_func, e_boolean, 'X',"eXit","after first read error\t\0",false,1200, 0.0,""},
#define EXIT_ON_FIRST_ERROR opts[18].b

#ifndef WITHOUT_RTS_DTR
	{crtscts_function,e_boolean,'c',"crtscts","operate RTS line\t\0",false,0,0.0,"n"},
#define bSerialRTS opts[19].b
#endif
#endif

	{empty_func,e_unknown,'\0',"\0","\0",0,0,0.0,"\0"}
};


// #define	WITH_DEBUG_	1


bool serial_config_globals()
{
	if (!lateConfigure) {
		return true;
	}

	if (!serial_configure_ext(SFD, BAUD, 8, 1, 'n', false)) {
		fprintf(stderr, "serial_configure_ext(): failed\n");
		return false;
	} else {
		if (tty_serial_timeout_set(SFD, 1)) {
			if (verbose_mode) {
				fprintf(stderr,"setting blocking IO for %d deciseconds\n", 1);
			}
			if (!serial_configure_par(SFD, 'n')) {
				fprintf(stderr,"cant set none parity\n");
			}
		} else {
			fprintf(stderr,"setting receiving timeout failed\n");
		}
	}
	lateConfigure = false;
	return true;
}


#define CHECK_SERIAL_FD_BOOL(_fn_)		\
		if (SFD < 0) { \
			fprintf(stderr,"%s: serial port not open\n", _fn_);	\
			return false;	\
		} else {	\
			if (!serial_config_globals()) return false;\
		}



#define CHECK_SERIAL_FD(_fn_)		\
		if (SFD < 0) { \
			fprintf(stderr,"%s: serial port not open\n", _fn_);	\
			return;	\
		} else {	\
			serial_config_globals();\
		}

void cpu_function(int i, double f, char *s)
{
    if (s != NULL) {
	if (strlen(s)>0) {
	    for (uint8_t j=0; j<sizeof(cpu_firmware_addr)/sizeof(cpu_firmware_addr_t); j++) {
		if (strcmp(s, cpu_firmware_addr[j].variantStr) == 0) {
		    CPU_VARIANT = cpu_firmware_addr[j].idx;
		    iNeedbytes	= cpu_firmware_addr[j].ramSize;
		    RAM_OFFS = cpu_firmware_addr[j].ramStart;
		    fprintf(stderr,"cpu variant set to index [%d] from string arg\n", CPU_VARIANT);
		    return;
		}
	    }
	}
    }
    if ((unsigned)i<sizeof(cpu_firmware_addr)/sizeof(cpu_firmware_addr_t)) {
	strcpy(CPU_VARIANT_NAME, cpu_firmware_addr[i].variantStr);
	iNeedbytes = cpu_firmware_addr[i].ramSize;
	RAM_OFFS = cpu_firmware_addr[i].ramStart;
	fprintf(stderr,"cpu variant set to index [%d] from int arg\n", CPU_VARIANT);
	return;
    }
    fprintf(stderr,"argument --cpu ignored using type [%d]\n", CPU_VARIANT);
}


void prefix_function(int i, double f, char *s)
{
	char *x = strchr(s,'x');

// TODO: get number of keybytes from CPU definition
#define	N_CPU_KEYBYTES		8

	int l = N_CPU_KEYBYTES;

	if (fexist(s)) {
		S19KeyFile keyFile;
		if (keyFile.Init((mc86hc908_variants_e)CPU_VARIANT, s)) {
			if (verbose_mode) {
				fprintf(stderr, "loading security key from file [%s]\n", s);
			}
			uint8_t *keyFileBytes = keyFile.getKeybytes();
			if (keyFileBytes != NULL) {
				memcpy(_SKEY_BYTES, keyFileBytes, keyFile.nKeys);
				goto prefix_found_keys;
			}
		} else {
			if (verbose_mode) {
				fprintf(stderr, "no key found in file [%s]\n", s);
			}
		}
	}

	if (x != NULL) {
		// hex mode
		int n = strlen(&s[2])/2;
		if (n > N_CPU_KEYBYTES) {
			n = N_CPU_KEYBYTES;
		}

		unsigned char sbuf[8];

		char parsed[N_CPU_KEYBYTES*4];
		memset(parsed,0,N_CPU_KEYBYTES*4);

		for (int i=0; i<n; i++) {
			sbuf[i] = C_S19rec::Hex2Byte(&s[i*2+2]);
			if (verbose_mode) {
				char hex[4];
				sprintf(hex,"%02X", sbuf[i]);
				strcat(parsed,hex);
//				fprintf(stderr,"parsed keybyte[%d] = %02X from s[%d]=[%s]\n", i, sbuf[i], i*2, &s[i*2+2]);
			}
		}
		if (verbose_mode) {
			fprintf(stderr,"parsed keybytes [%s]\n", parsed);
		}

		memcpy(_SKEY_BYTES, sbuf, N_CPU_KEYBYTES);
	} else {
		x = strchr(s,'-');
		if (x != NULL) {	// -1 aka 0xFFFFFFFFFFFFFFFF mode
			memset(_SKEY_BYTES, 0xFF, N_CPU_KEYBYTES);
		} else {
			// string mode
			l = strlen(s);
		}
	}

prefix_found_keys:
	for (int i=0; i<N_CPU_KEYBYTES; i++) {
		fprintf(stderr,"%02X", _SKEY_BYTES[i]);
	}
	fprintf(stderr,"\n");
}




//____________
// raw monitor functions
// *****
// write_memory_block
// run user program
// read sp

//____________
// user space functions
// *****
// DONE: toggle dtr or rts
// DONE: delay
// DONE: read chunk binary with offset and size via Monitor to filename
// DONE: read stack pointer
//
// send prefix
// send binary file via Monitor to RAM and execute
// send binary file

// TODO: load a S-Record file and poke memory RAS

int consume_bytes()
{
	int ret = tcflush(SFD, TCIOFLUSH);
	if (ret < 0) {
		fprintf(stderr,"%s: tcioflush: %s\n", __func__, strerror(errno) );
	}
	return ret;
/*
	int r = 0;
	if (FD_ReadyTo(SFD, 300000)) {		// 0.3 sec
		fprintf(stderr,"unblock IO\n");
		if (fcntl(SFD,F_SETFL,O_NDELAY) < 0) {
			fprintf(stderr,"cant flush uart receiver: %d, %s\n", errno, strerror(errno));
		} else {
			char buf[32];
			r = read(SFD, buf, 32);
			if (r > 0) {
				char numB[64],digitB[16];
				memset(numB, 0, 64);
				for (int i=0; i<r; i++) {
					if( strlen(numB) > 0) {
						strcat(numB," ,");
					}
					sprintf(digitB,"0x%02X", buf[i]);
					strcat(numB, digitB);
				}
				fprintf(stderr,"eat up %d bytes (%s) from receiver\n", r, numB);
			} else {
				// nothing to read
			}
			fprintf(stderr,"set blocking IO\n");
			if (fcntl(SFD,F_SETFL,O_SYNC) < 0) {
			}
		}
		return r;
	} else {
		fprintf(stderr,"no bytes to consume\n");
		return 0;
	}
*/
}


static int last_CTG = 0;

bool monitor_raw_cmd(hc908_monitor_indices_e idx, uint8_t *_DataBuf)
{
	unsigned char serial_buf[16];
	unsigned char sbuf[8];
	unsigned long blockFillDelay;
#ifdef WITH_DEBUG_
	fprintf(stderr,"%s(%d)\n", __func__, idx);
#endif
	CHECK_SERIAL_FD_BOOL(__func__);

//	short i = 0;
	hc908_monitor_cmd_t *monitor = &hc908_monitor_cmd[idx];

	int ir = 0;
	int is = 0;

//	consume_bytes();

	memset(serial_buf, 0, 16);

	int toGet = monitor->nReceive
				+ (monitor->nSend*2);
				// one byte sent, one byte looped via wired_OR, one byte echoed by MCU

	int CTG = 5 + GAP_EXTRA;
	// (chars-to-gap)
	// (one byte for the echo, one for MCU thinking time) times two plus user value

	long oneCharDelay = buffer_delay(1, BAUD, 8, 1, 'n');

	blockFillDelay = buffer_delay(CTG, BAUD, 8, 1, 'n');

	if (verbose_mode) {
		if (CTG != last_CTG) {
			fprintf(stderr,"extra buffering delay %ld to safely queue %d chars\n", blockFillDelay, CTG);
			last_CTG = CTG;
		}
	}

	if (idx == eSendKeyBytes) {
//		memcpy(_DataBuf, SKEY_BYTES, 8);
//		uint64_t *lKey = (uint64_t *)&_DataBuf;
		fprintf(stderr, "sendKeyBytes 0x");
		for (int i=0; i<N_CPU_KEYBYTES; i++) {
			if (_SKEY_BYTES[i] != _DataBuf[i]) {
				fprintf(stderr,"^");
				_DataBuf[i] = _SKEY_BYTES[i];
			}
			fprintf(stderr,"%02X", _DataBuf[i]);

		}
		fprintf(stderr,"\n");

	}
//#ifdef WITH_DEBUG_
//#endif
//	int i_ = ir+is;

//	if (verbose_mode) {
//	}
#ifdef WITH_DEBUG_
	if (verbose_mode) {
		fprintf(stderr,"r:%d s:%d m:%d\n",  monitor->nSend,  monitor->nReceive, toGet);
	}
#endif
	while (ir < toGet) {

		int w = 0;
		int r = 0;

		if (is < monitor->nSend) {

			if (idx == eSendKeyBytes) {
				sbuf[is] = _DataBuf[is];
			} else {
				if (is == 0) {
					sbuf[0] = monitor->cmd;
				} else {
					sbuf[is] = _DataBuf[is-1];
				}
			}
#ifdef WITH_DEBUG_
			fprintf(stderr,"write() sbuf[%d]:=x%02X\n", is, sbuf[is]);
#endif
			w = write(SFD, &sbuf[is], 1);
			int ret = tcdrain(SFD);
			if (ret < 0) {
				fprintf(stderr,"%s: tcdrain: %s\n", __func__, strerror(errno) );
			}
			if (w != 1) {
				fprintf(stderr,"%s step %d  write(sbuf[%d]=%d) %s\n", __func__, ir, is, sbuf[is], strerror(errno));
				return false;
			}
		}

		if (w > 0) {
			unsigned char looped = 0xFF;
			unsigned char echoed = 0xFF; // TODO: security bytes are probably not echoed

			// must receive 2 bytes, one was looped via wired-OR, one was echoed by the MCU
			if (FD_ReadyTo(SFD, blockFillDelay)) {
				usleep(oneCharDelay);

				r = read(SFD, &serial_buf[ir], 2);
				if (r < 2) {
					fprintf(stderr, "step %d,%d, %s byte missing, r=%d, probably %s, %s rbuf[%d]=0x%02X\n", is, ir
						, r==0 ? "looped" : "echoed"
						, r
						, r==1 ? "MCU not in Monitor Mode" : "programmer not connected"
						, strerror(errno), ir, serial_buf[ir]);
					return false;
				}
			} else {
				fprintf(stderr,"timed out in waiting for echoed char");
				if (verbose_mode && (is == 0)) {
					fprintf(stderr," Cable problems?\n");
				} else {
					fprintf(stderr,", it:%d\n", is);
				}
				return false;
			}
#ifdef WITH_DEBUG_
			fprintf(stderr,"read() rbuf[%d]:=x%02X%02X\n", ir, serial_buf[ir], serial_buf[ir+1]);
#endif
			// got looped
			looped = serial_buf[ir];
			echoed = serial_buf[ir+1];

			ir += 2;

			if ((looped != sbuf[is]) || (echoed != sbuf[is]) || (looped != echoed))  {
				fprintf(stderr,"step %d,%d, invalid character received 0x%02x, 0x%02x, 0x%02X\n", is, ir, sbuf[is], looped, echoed);
				return false;
			}
			is++;
		} else {
			if (ir < toGet) {

				int trail = toGet-ir;
//				int trail = 1;

				long getDelay = blockFillDelay;
				if (trail < 2) {
					getDelay = oneCharDelay;
				}
				if (FD_ReadyTo(SFD, getDelay)) {
					r = read(SFD, &serial_buf[ir], 1);

					if (r == 1) {
						// OK
					} else {
						// error
						if (r == 0) {
							// should have blocked
							continue;
						} else {
							fprintf(stderr,"step[%d,%d], %d byte(s) expected, got %d, %s\n", is , ir, trail, r, strerror(errno));
							return false;
						}
					}
				} else {
					fprintf(stderr,"step[%d,%d], timed out in waiting for %d received char(s)\n", is , ir, trail);
					return false;
				}

#ifdef WITH_DEBUG_
				fprintf(stderr,"read() rbuf[%d]:=x%02X\n", ir, serial_buf[ir]);
#endif
				ir++;


			} else {
				fprintf(stderr,"BUG!! nothing written and nothing to receive !!  incrementing ir\n");
				ir++;
			}
		}
#ifdef WITH_DEBUG_
		if (verbose_mode) {
			fprintf(stderr,"%d, %d 0x%02X  0x%02x, 0x%02x\n", is, ir, monitor->cmd, sbuf[is], serial_buf[ir]);
		}
#endif
	}
	switch(idx) {
		case eReadMem:
			_DataBuf[0] = serial_buf[6];
			break;

		case eIncRead:
		case eReadSP:
			_DataBuf[0] = serial_buf[2];
			_DataBuf[1] = serial_buf[3];
			break;

		default:
			break;
	}
	return true;
}





bool monitor_poke_byte(uint16_t addr, uint8_t value)
{
	uint8_t buf[8];
	memset(buf, 0, 8);

	buf[0] = (addr >> 8) & 0xFF;
	buf[1] = addr & 0xFF;
	buf[2] = value;

	if (!monitor_raw_cmd(eWriteMem, buf)) {
		return false;
	}
	return true;
}

bool monitor_poke_word(uint16_t addr, uint16_t value)
{
	if (!monitor_poke_byte(addr, (value >> 8))) {
		return false;
	}

	uint8_t buf[8];
	memset(buf, 0, 8);
	buf[0] = value & 0xFF;

	return monitor_raw_cmd(eIncWrite, buf);
}


bool monitor_write_ram(uint16_t addr, int len, uint8_t *buffer)
{
	if (!monitor_poke_byte(addr, buffer[0])) {
		return false;
	}

	uint8_t buf[8];

	for (int i=1; i<len; i++) {
		memset(buf, 0, 8);
		buf[0] = buffer[i];

		if (!monitor_raw_cmd(eIncWrite, buf)) {
			return false;
		}
	}

	return true;
}

int read_monitor_mem(int addr)
{
	unsigned char cBuf[8];

	cBuf[0] = (addr >> 8) & 0xFF;
	cBuf[1] = addr & 0xFF;

	bool result = monitor_raw_cmd(eReadMem, cBuf);
	if (!result) {
		return -1;
	}
	return cBuf[0];
}


bool print_hash(int i, int full)
{
	bool result = false;

	if ((i & bHashPrintModulo) == 0) {
		result = true;

		if (bHashPrinting) {
// TODO: print ETA in minutes:seconds
			int percent = (i * 100) / full;
			fprintf(stdout,"\b\b\b\b#%3d%%", percent);
			fflush(stdout);
		}
	}

	return result;
}




bool write_file_to_ram(char *binFileName, int offset, int size)
{
	int addr = 0;
	unsigned char buf[8];
	memset(buf, 0, 8);

	int sent = 0;

	int bfd = open(binFileName, O_RDONLY);
	if (bfd < 0) {
		fprintf(stderr, "open(%s) %s\n", binFileName, strerror(errno) );
		return false;
	}

	buf[0] = (offset >> 8) & 0xFF;
	buf[1] = offset & 0xFF;

	bool result = false;

	int r = read(bfd, &buf[2], 1);
	if (r != 1) {
		fprintf(stderr, "read(%s) %s\n", binFileName, strerror(errno) );
		goto _exit_write_ram;
	}

	if (!monitor_raw_cmd(eWriteMem, buf)) {
		fprintf(stderr,"first write failed\n");
		goto _exit_write_ram;
	}

	sent++;
	result = false;
	do {
		if (sent >= size) { // finished
			result = true;
			break;
		}
		r = read(bfd, &buf[0], 1);
		if (r == 1) {
			// file read OK
			result = monitor_raw_cmd(eIncWrite, buf);
			if (!result) {
				fprintf(stderr,"%d. write failed\n", sent);
				break;
			}
			// send byte OK
			sent++;
			print_hash(sent, size);
		} else {
			fprintf(stderr,"read(): %d %s\n", errno, strerror(errno) );
			break;
		}
	} while(true);

_exit_write_ram:
	close(bfd);
	return result;
}



static uint8_t propellerIdx = 0;
const static char propellerChar[] = "\\|/-\\|/-";

void writePropeller()
{
	fprintf(stderr,"%c\b", propellerChar[propellerIdx]);
	fflush(stderr);
	propellerIdx++;
	propellerIdx &= 0x07;
}

bool write_bootLoader(int cpuIdx)
{
	if ((cpuIdx >= mc68hc908_undefined) || (cpuIdx < 0)) {
		fprintf(stderr,"unimplemented CPU variant %d\n", cpuIdx);
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
		return false;
	}

	const cpu_firmware_addr_t *fwAddr = &cpu_firmware_addr[cpuIdx];

	char bootLoaderFile[80];
	sprintf(bootLoaderFile,"%s-bootloader.bin", fwAddr->variantStr);
	if (verbose_mode) {
		fprintf(stderr,"loading flash programming bootloader \"%s\"\n", bootLoaderFile);
	}
	if (!write_file_to_ram(bootLoaderFile, fwAddr->ramStart, fwAddr->ramSize)) {
		return false;
	}
	return true;
}


// write/erase the flash with the help of firmware (aka monitor ROM)
bool write_flash_fw(mc86hc908_variants_e idx, int MHz, uint16_t startAddr, int len, uint8_t *_buffer)
{
	bool doErase = ((len < 0) && (_buffer == NULL) && (startAddr == 0));

	// selection for different CPU types
	const cpu_firmware_addr_t *fwAddr = &cpu_firmware_addr[idx];

	// check bootloader presence and reload if necessary
	int magic = read_monitor_mem(fwAddr->jumpAddr-2);
	if (magic < 0) {
		fprintf(stderr,"read address 0x%04X failed\n",fwAddr->jumpAddr-2);
		return false;
	} else {
//		if (verbose_mode) {
//			fprintf(stderr,"got magic [%02x] from CPU\n", magic);
//		}
		if (magic == fwAddr->HX) {
//			if (verbose_mode) {
//				fprintf(stderr,"magic matches [%02x], monitor is probably present\n", fwAddr->HX);
//			}
			//
		} else {
			if (verbose_mode) {
				fprintf(stderr,"reload bootloader on-the-fly\n");
				if (!write_bootLoader(idx)) {
					fprintf(stderr,"writing bootloader failed\n");
					return false;
				}
				fprintf(stderr,"\nstarting flash\n");
			}
		}
	}

	if (!doErase) {
// #ifdef STRICT_FLASH_ADDRESS
		if (((startAddr & 0xFF) + len) > 256) {
			fprintf(stderr,"start-address(0x%04X) + length(%d)  crosses 256 byte boundary to 0x%04X, arg:0x%04X, align your S19 file properly\n"
					,startAddr, len, startAddr+len, ((startAddr & 0xFF) + len));
			return false;
		}
// #endif

		int endAddr = startAddr+len;

		if (endAddr > (fwAddr->flashStart + fwAddr->flashSize)) {
			if (endAddr > fwAddr->vectorStart) {
				if (endAddr > 0x10000) {
					fprintf(stderr,"program length 0x%04X exceeds end-of vector table\n", endAddr);
					return false;
				}
			} else {
				fprintf(stderr,"program length 0x%04X exceeds end-of flash 0x%04X\n", startAddr+len, fwAddr->flashStart + fwAddr->flashSize);
				return false;
			}
		}

		// clear mass erase bit
		if (!monitor_poke_byte(fwAddr->CtrlByte, 0)) {
			fprintf(stderr,"setting CtrlByte failed\n");
			return false;
		}
		// poke flash program address
		if (!monitor_poke_word(fwAddr->jumpAddr, fwAddr->program)) {
			fprintf(stderr,"setting jump address to program failed\n");
			return false;
		}
	} else {
		fprintf(stderr, "\n=== !! FW-erase works only partially !! I don't know why ===\n\n");

		startAddr = 0xFFF0;		// fwAddr->flashStart;
//		len = fwAddr->flashSize;  // only check the first 16 bytes for erase
		len = 16;

		// poke mass erase bit
		if (!monitor_poke_byte(fwAddr->CtrlByte, (1<<6) )) {
			fprintf(stderr,"setting CtrlByte failed\n");
			return false;
		}
		if (verbose_mode) {
			fprintf(stderr,"CtrlByte[0x%04x] poked to [0x%04x] in monitor RAM",fwAddr->CtrlByte,(1<<6));
		}
		// poke flash erase address
		if (!monitor_poke_word(fwAddr->jumpAddr, fwAddr->erase)) {
			fprintf(stderr,"setting jump address to erase failed\n");
			return false;
		}
		if (verbose_mode) {
			fprintf(stderr,"jumpAddr[0x%04x] poked to [0x%04x] in monitor RAM",fwAddr->jumpAddr, fwAddr->erase);
		}
	}

	uint16_t lastAddr = startAddr +len -1;

	if (startAddr < fwAddr->flashStart) {
		fprintf(stderr,"start-address 0x%04X is lower than flash start 0x%04X\n", startAddr, fwAddr->flashStart);
		return false;
	}

	if (startAddr > (fwAddr->flashStart + fwAddr->flashSize)) {
		if (startAddr < fwAddr->vectorStart) {
			if (lastAddr > fwAddr->vectorStart) {
				int moveDelta = fwAddr->vectorStart - startAddr;
				if (moveDelta > 64) {
					fprintf(stderr,"illegal start address 0x%04X\n", startAddr);
					return false;
				}

				// sort bytes out
				int k = 0;
				for (int n=fwAddr->vectorStart; n<0x10000; n++) {
					_buffer[k] = _buffer[k+moveDelta];
					k++;
				}
				startAddr += moveDelta;
				len -= moveDelta;

				fprintf(stderr,"skipping preceeding %d bytes before vector table @ 0x%04X\n", moveDelta, fwAddr->vectorStart);
			} else {
				fprintf(stderr,"start-address 0x%04X is lower than vector start 0x%04X\n", startAddr, fwAddr->vectorStart);
				return false;
			}
		}
	}

	if (!doErase) {
		if (!monitor_write_ram(fwAddr->dataBuf, len, _buffer)) {
			fprintf(stderr,"writing bytes failed\n");
			return false;
		}
	}

	if (!monitor_poke_word(fwAddr->Laddr, lastAddr)) {
		fprintf(stderr,"setting last-address failed\n");
		return false;
	}

	if (!monitor_poke_byte(fwAddr->cpuSpeed, MHz*4)) {
		fprintf(stderr,"setting CPU-speed failed\n");
		return false;
	}
	if (!monitor_poke_word(fwAddr->HX, startAddr)) {
		fprintf(stderr,"setting start-address failed\n");
		return false;
	}
	if (!monitor_poke_byte(fwAddr->FLBPR, 0xFF)) {
		fprintf(stderr,"unprotect flash failed\n");
		return false;
	}
//#ifdef WITH_DEBUG_
	if (verbose_mode) {
		fprintf(stderr,"executing bootloader in RAM\n");
	}
//#endif
	uint8_t rBuf[32*1024];

	if (! monitor_raw_cmd(eExec, rBuf)) {
		fprintf(stderr,"executing bootloader in RAM failed\n");
		return false;
	}

	bool result = false;

	// wait for finish
	// set nonblocking IO on
	fcntl(SFD, F_SETFL, O_NDELAY);

	int pIdx = 0;
	int rIdx = 0;
	while(rIdx < 96) {
		if (FD_ReadyTo(SFD, 2000)) {
			int r = read(SFD, &rBuf[rIdx], 64);
			if (r > 0) {
				rIdx += r;
//				for (int i=0; i<r; i++) {
//					fprintf(stdout,"0x%02X ",rBuf[rIdx+i]);
//				}
//				fflush(stdout);
			}
		}
		if (FD_ReadyTo(0, 2000)) {
			char _buf[32];
			int r = read(0, _buf, 32);
			if (r > 0) {
				_buf[r] = '\0';
				if (_buf[r-1] == '\n') {
					_buf[r-1] = '\0';
				}
				if (verbose_mode) {
					fprintf(stderr,"got command [%s] rIdx:%d\n", _buf, rIdx);
				}
				if (strcmp(_buf,"/quit") == 0) {
					exit_proc(0);
				}
				if (strcmp(_buf,"/exit") == 0) {
					exit_proc(7);
				}
				if (strcmp(_buf,"/continue") == 0) {
					result = true;
					break;
				}
				if (strcmp(_buf,"/break") == 0) {
					break;
				}
			}
		}

		if (rIdx >= (len +1)) {
			if (rBuf[len] == 'R') {	// got last char from bootloader-helper asm
				result = true;

				// verify results
				for (int i=0; i<len; i++) {
					if (!doErase) {
						if (_buffer[i] != rBuf[i]) {
							fprintf(stderr,"flash verify failed %dth char @ 0x%04X  sent:0x%02X != received:0x%02X\n", i, startAddr, _buffer[i], rBuf[i]);
							result = false;
							break;
						}
					} else {
						if (rBuf[i] != 0xFF) {
							fprintf(stderr," %dth char @ 0x%04X  not deleted 0x%02X\n", i, startAddr, rBuf[i]);
							result = false;
							break;
						}
					}
				}
			} else {
				fprintf(stderr, "got %d bytes but wrong marker 0x%02X, [", len, rBuf[len]);
				for (int i=0; i<len; i++) {
					fprintf(stderr,"0x%02X,", rBuf[i]);
				}
				fprintf(stderr, "\n");
			}
			break;
		} else {
			// every written byte comes back also here, written by the bootloader code
			// after the return from the firmware subroutine
			// we went here since the amount of needed data was not yet fulfilled

			if (rIdx == fwAddr->flashSize) {
				// probably erase-routine returned here
			}

//			for (int i=pIdx; i<rIdx; i++) {
//				writePropeller();
//			}
//			if (len != rIdx) {
//				fprintf(stderr, "got only %d bytes, [", rIdx);	// why are that so much zeros in here
#ifdef WITH_DEBUG_
				for (int i=0; i<len; i++) {
					fprintf(stderr,"0x%02X,", rBuf[i]);
				}
#endif
//				fprintf(stderr, "]\n");
//			}
		}
	}
	fcntl(SFD, F_SETFL, O_SYNC);
/*
	if (verbose_mode) {
		if (!result) {
			fprintf(stderr,"flash verify failed\n");
		}
	}
*/
	return result;
}


// TODO: toggle power-on reset
// TODO: wait for short time
// TODO: send byte train, there will be no response

void waitBreak_function(int i, double f, char *s)
{
	// wait for break signal
	uint8_t rbuf[4];

	if (serial_configure_par(SFD, 'e')) {
		if (FD_ReadyTo(SFD, BREAK_WAIT_MSEC*1000)) {
			int r = read(SFD, &rbuf[0], 1);
			if (rbuf[0] == 0) {
				if (!serial_configure_par(SFD, 'n')) {
					fprintf(stderr,"cant set none parity\n");
				}
				return;
			}
		}
		if (verbose_mode) {
			fprintf(stderr,"waiting for break timed out after %d ms\n", BREAK_WAIT_MSEC);
		}
		if (!serial_configure_par(SFD, 'n')) {
			fprintf(stderr,"cant set none parity\n");
		}
		if (bSendKeybytes) {
			uint64_t *lKey = (uint64_t *)&_SKEY_BYTES;
#ifdef __i686__			
			fprintf(stderr, " -> arg key3 0x%016llX  @ 0x%08lx\n", *lKey, (long)lKey);
#else			
			fprintf(stderr, " -> arg key3 0x%016lX  @ 0x%016lx\n", *lKey, (long)lKey);
#endif			

//			lKey = (uint64_t *)&security_key;
//			fprintf(stderr, " -> arg key4 0x%016llX  @ 0x%08lx\n",*lKey, (long)lKey);

			if (!monitor_raw_cmd(eSendKeyBytes, _SKEY_BYTES)) {
				exit_proc(6);
			}

		} else {
			exit_proc(5);
		}
	} else {
		fprintf(stderr,"cant set even parity\n");
	}
}




void readVar_function(int i, double f, char *s)
{
	int result = read_monitor_mem(READ_VAR_ADDR);
	if (result >= 0) {
		fprintf(stderr, "MEM[0x%04X] := 0x%02X\n", READ_VAR_ADDR, result);
	}
}

// TODO: write s19 file

bool read_ram(char *binFileName, int offset, int size)
{
	int addr = 0;
	unsigned char fbuf[64*1024];
	memset(fbuf, 0, 64*1024);

	int bfd = open(binFileName, O_RDWR|O_CREAT|O_TRUNC, 0644);
	if (bfd < 0) {
		fprintf(stderr, "open(%s) %s\n", binFileName, strerror(errno) );
		return false;
	}

	unsigned char cBuf[8];

	cBuf[0] = (offset >> 8) & 0xFF;
	cBuf[1] = offset & 0xFF;

	bool result = monitor_raw_cmd(eReadMem, cBuf);
	if (!result) {
		return false;
	}

	fbuf[0] = cBuf[0];

//	write(bfd, fbuf, 1);
	int stop = size;
	int k = 1;
	int w = 1;
	int size2 = (size / 2);


	for (int i=0; i<size2; i++) {
		result = monitor_raw_cmd(eIncRead, cBuf);

		if (!result) {
			write(bfd, fbuf, k);
			close(bfd);
			return false;
		}
//		unsigned char fbuf[2];
//		memcpy(fbuf, buf, 2);


		fbuf[k] = cBuf[0];
		fbuf[k+1] = cBuf[1];

		k += 2;
		w += 2;

		if (print_hash(i*2, size)) {
			write(bfd, fbuf, k);
			k = 0;
		}

#ifdef WITH_DEBUG
//		fprintf(stderr, "i:%d  k:%d  s:%d   0x%02X%02X\n",i, k, stop,  fbuf[0], fbuf[1]);
#endif
		if (w >= stop) {
//		if (((i+1)*2) >= stop) {
//			write(bfd, fbuf, 1);
			w--;
			k--;
			break;
		} else {
//			write(bfd, fbuf, 2);
		}
	}

	write(bfd, fbuf, k);
	close(bfd);

	if (verbose_mode) {
		fprintf(stderr,"\b\b\b\b got %d bytes into file [%s]\n", w, binFileName);
	}
	return true;
}




bool flash_s19_file(char *s19FileName, int cpuIdx, int MHz)
{
/*

*/
	// upload flash-program-helper
	if (needBootLoader) {
		if (!write_bootLoader(cpuIdx)) {
			return false;
		}
	}

	t_buffer FBuf;
	if (verbose_mode) {
		fprintf(stderr,"open s19 format file \"%s\"\n", s19FileName);
	}
	if (FBuf.Init(s19FileName, 128*1024) == false) {
		fprintf(stderr,"open %s failed, %s\n", s19FileName, strerror(errno));
		return false;
	}

	int lineCnt = 1;
	char *L = FBuf.ReadLn();
	while (L != NULL) {
		t_s19Line S1Line,*got;

		uint8_t lineCheck = 0;
		got = C_S19rec::Filled(L, &S1Line, &lineCheck);

		if (got != NULL) {
#ifdef WITH_DEBUG_
//			fprintf(stderr,"got.addr:%ld, got.buffer:%ld, got.Len:%ld\n",got->Addr, (long)&got->buffer[0], got->Len);
#endif
			switch(got->ID) {
				case '0':
					if (verbose_mode) {
						fprintf(stderr," S19-header: [");
						// TODO: split into name, version, revision, comment
						for (uint8_t i=0; i<got->payloadLen; i++) {
							fprintf(stderr,"%c", got->buffer[i]);
						}
						fprintf(stderr,"]\n");
					}
					break;

				case '1':
				case '2':
				case '3':
					if ((lineCheck == 0xFF) && (got->ID)) {
//						if (verbose_mode) {
//							fprintf(stderr,"flashing line [%s]\n", L);
//						}
						if (!write_flash_fw((mc86hc908_variants_e)cpuIdx, MHz, got->Addr, got->payloadLen, got->buffer)) {
							C_S19rec::chomp(L);
							fprintf(stderr, "line %d [%s] not flashed\n", lineCnt, L);
							FBuf.Done();
							return false;
						} else {
							fprintf(stdout,"\b\b\b\b\b\b\b\b#  %3d%%  ", (int)((FBuf.Offs()*100) / FBuf.fsize));
							fflush(stdout);
							lineCnt++;
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
				fprintf(stderr,"skipping invalid S19line [%s]\n", L);
		    }
//#endif
		}
		L = FBuf.ReadLn();
	}
	FBuf.Done();


	S19KeyFile keyFile;
	if (keyFile.Init((mc86hc908_variants_e)CPU_VARIANT, s19FileName)) {
		// there is a key in this file
/*
		if (verbose_mode) {
			fprintf(stderr, "loading security key from file [%s]\n", s);
		}
*/
		uint8_t *keyFileBytes = keyFile.getKeybytes();
		if (keyFileBytes != NULL) {
			// TODO: let the user choose between binary, s19 or both

			if (BACKUP_FC_MASK & 0x04) {
				int lkf = open("lastflashed-key.bin",O_RDWR|O_CREAT|O_TRUNC,0644);
				if (lkf >= 0) {
					write(lkf, keyFileBytes, N_CPU_KEYBYTES);
				}
				close(lkf);
			}

			if (BACKUP_FC_MASK & 0x02) {
				int lkf = open("lastflashed-key.s19",O_RDWR|O_CREAT|O_TRUNC,0644);
				if (lkf >= 0) {
					S19ChkSum ChkSum(s19_16bitAddr);
					char s19buf[256];

					char *s19line = ChkSum.ArrayToS19(keyFile.firstAddr, keyFileBytes, N_CPU_KEYBYTES, s19buf);
					if (s19line != NULL) {
						strcat(s19line,"\n");
						write(lkf, s19line, strlen(s19line));
					}
				}
				close(lkf);
			}
		}
	}

	if (BACKUP_FC_MASK & 0x01) {	// let the user switch this off
		char backupCmd[256];
		sprintf(backupCmd, "cp -f -p -b %s lastflashed-backup.s19", s19FileName);
		system(backupCmd);
	}
	return true;
}



void tty_function(int i, double f, char *s)
{
	uint8_t buf[64];

	// set nonblocking IO on
	fcntl(SFD, F_SETFL, O_NDELAY);

	while(true) {
		if (FD_ReadyTo(SFD, 2000)) {
			int r = read(SFD, buf, 8);
			if (r > 0) {
				for (int i=0; i<r; i++) {
					fprintf(stdout,"0x%02X ",buf[i]);
				}
				fflush(stdout);
			}
		}
		if (FD_ReadyTo(0, 2000)) {
			memset(buf, 0, 64);
			int r = read(0, buf, 64);
			if (r > 0) {
				buf[r] = '\0';
				if (buf[r-1] == '\n') {
					buf[r-1] = '\0';
				}
				if (verbose_mode) {
					fprintf(stderr,"got command [%s]\n", buf);
				}
				// - hex-mode on/off
				// - parity on/off
				// - send hex bytes
				// - send string
				// - quit tty
				// - exit program
			}
		}
	}

	fcntl(SFD, F_SETFL, O_SYNC);
}



void exec_function(int i, double f, char *s)
{
	CHECK_SERIAL_FD(__func__);

	unsigned char buf[8];
	if (! monitor_raw_cmd(eExec, buf)) {
		exit_proc(3);
	}
	if (verbose_mode) {
		fprintf(stderr,"executed rti\n");
	}
}




void send_function(int i, double f, char *s)
{
	CHECK_SERIAL_FD(__func__);
	fprintf(stderr,"TODO: unimplemented\n");
}


/*
long blockFillDelay = 0;

void buffer_delay_func(int i, double f, char *s)
{
	// block delay count arg was given on command-line
	int nBits = iSerialBits;	// care for 5,6,7,8 bit data modes
	nBits += iSerialStop;		// count extra
	if (tolower(iSerialParity) != 'n') {
		nBits += iSerialStop;
	}
	// int uSleep = (iBlockRecv * 1000000 * nBits) / Baud;    // 		chars  /  (byte per second)
	blockFillDelay = 1000000;
	blockFillDelay *= iBlockRecv;
	blockFillDelay *= nBits;
	blockFillDelay /= BAUD;

	if (verbose_mode) {
		fprintf(stderr,"extra buffering delay %ld to safely queue %d chars\n", blockFillDelay, i);
	}
}
*/

void readCtrlLineBits(void)
{
	int serstat;
	if (verbose_mode) {
		if (ioctl(SFD, TIOCMGET, &serstat)<0) {
			perror("getMCRbits() TIOCMGET serstat");
		}
		if (serstat & TIOCM_RTS) {
			rtsbit = 1;
		} else {
			rtsbit = 0;
		}
		if (serstat & TIOCM_DTR) {
			dtrbit = 1;
		} else {
			dtrbit = 0;
		}
		fprintf(stderr,"rts is %d, dtr is %d\n",rtsbit, dtrbit);
	}
}


void toggle_function(int i, double f, char *s)
{
	CHECK_SERIAL_FD(__func__);

	int serstat;
	if (ioctl(SFD, TIOCMGET, &serstat)<0) {
		perror("getMCRbits() TIOCMGET serstat");
	}
	if (serstat & TIOCM_RTS) {
		rtsbit = 1;
	} else {
		rtsbit = 0;
	}
	if (serstat & TIOCM_DTR) {
		dtrbit = 1;
	} else {
		dtrbit = 0;
	}
	if 	((i & 8) == 8) {
		// rts
		if (verbose_mode) {
			fprintf(stderr,"modify rts\n");
		}
		if ((i & 2) == 2) {
			serstat |= TIOCM_RTS;
		} else {
			serstat &= ~TIOCM_RTS;
		}
	}
	if 	((i & 4) == 4) {
		// dtr
		if (verbose_mode) {
			fprintf(stderr,"modify dtr\n");
		}
		if ((i & 1) == 1) {
			serstat |= TIOCM_DTR;
		} else {
			serstat &= ~TIOCM_DTR;
		}
	}
	if (ioctl(SFD, TIOCMSET, &serstat)<0) {
		perror("toggle_function(): TIOCMSET");
	}
	readCtrlLineBits();
}


void gap_delay_function(int i, double f, char *s)
{
	usleep(i*1000);
}

void open_function(int i, double f, char *s)
{
	long baudKey = requestedSpeedKey(BAUD);
	if (baudKey < 0) {
		if (!setCustomSerialSpeed(BAUD, PORT)) {
			fprintf(stderr,"setting customized baud divider failed\n");
		} else {
			if (verbose_mode) {
				fprintf(stderr,"\tsetting customized baud divider OK\n");
			}
		}
#ifdef WITH_DEBUG_
	} else {
		if (verbose_mode) {
			fprintf(stderr,"baud key is 0x%08lx %s\n",baudKey, (baudKey<0)?"negative":"positive");
		}
#endif
	}
	SFD = open(PORT, O_RDWR | O_NOCTTY | O_SYNC);
}


void reset_function(int i, double f, char *s)
{
	// -reset    =>   -k key -g 2 -t 4 -w 20 -a 0x80 -R bootloader.bin

	// load key from keyfile
	prefix_function(i, f, SKEY_BYTE_ARG);

	if (SFD >= 0) {
		close(SFD);
		SFD = -1;
	}
	open_function(i, f, PORT);

	if (IS_POR_RESET) {
		// hold the reset line low
		toggle_function(TOGGLE_BITS | 1, f, s);

		// TODO: hold the power line low
		// TODO: wait for discharge
		// TODO: raise the power line
		fprintf(stderr," === !!! POWER-CYCLE your target quickly now !!! === \n");
		
		// wait extra time or shake the power
		gap_delay_function(POR_RESET_ARG, f, s);
	}

	lateConfigure = true;
	bSendKeybytes = true;

	CHECK_SERIAL_FD(__func__);

	gap_delay_function(GAP_TIMEOUT, f, s);

	toggle_function(TOGGLE_BITS, f, s);

	waitBreak_function(i, f, s);

	needBootLoader = true;
}



void minimal_open()
{
	int i =0;
	float f = 0.0;
	char *s = NULL;

	open_function(i, f, PORT);

	lateConfigure = true;

	CHECK_SERIAL_FD(__func__);

	toggle_function(TOGGLE_BITS, f, s);

	if (verbose_mode) {
		bHashPrinting = true;
	}
}

void erase_function(int i, double f, char *s)
{
	if (SFD < 0) {
		minimal_open();
	}
/*
	// upload flash-program-helper
	if (needBootLoader) {
		if (!write_bootLoader(CPU_VARIANT)) {
			return;
		}
	}
*/
	if (!write_flash_fw((mc86hc908_variants_e)CPU_VARIANT, CPU_MHZ, 0, -1, NULL)) {
		fprintf(stderr, "flash erase failed\n");
	}
}


void jump_function(int i, double f, char *s)
{
	if (SFD < 0) {
		minimal_open();
	}

	// upload flash-program-helper
	if (needBootLoader) {
		if (!write_bootLoader(CPU_VARIANT)) {
			return;
		}
	}

	const cpu_firmware_addr_t *fwAddr = &cpu_firmware_addr[CPU_VARIANT];

	if (!monitor_poke_word(fwAddr->jumpAddr, i)) {
		fprintf(stderr,"setting jump address to program failed\n");
		return;
	}
	uint8_t buf[16];
	if (! monitor_raw_cmd(eExec, buf)) {
		fprintf(stderr,"executing bootloader in RAM failed\n");
		return;
	}
}

void flashProg_function(int i, double f, char *s)
{
	if (SFD < 0) {
		minimal_open();
	}
	if (verbose_mode) {
		bHashPrinting = true;
	}
	if (!flash_s19_file(s, CPU_VARIANT, CPU_MHZ)) {
		exit_proc(6);
	}
}


void SP_function(int i, double f, char *s)
{
	if (SFD < 0) {
		minimal_open();
	}

	unsigned char buf[8];
	memset(buf,0,8);
	if (! monitor_raw_cmd(eReadSP, buf)) {
		exit_proc(4);
	}
	int sp =  ((buf[0] << 8) | buf[1]);
	fprintf(stdout,"got SP: 0x%04X %d\n", sp, sp);
}



void send_monitor_function(int i, double f, char *s)
{
	if (SFD < 0) {
		minimal_open();
	}

	long int L = filelen(s);
	if (L < 0) {
		fprintf(stderr,"file(%s) has some troubles, %s\n", s, strerror(errno) );
		return;
	};

	if (bNeedbytes) {
		if (iNeedbytes < L) {
			L = iNeedbytes;
			if (verbose_mode) {
				fprintf(stderr,"limiting send length to %ld\n", L);
			}
		}
	}
	if (verbose_mode) {
		bHashPrinting = true;
	}
	if (!write_file_to_ram(s, RAM_OFFS, (int)L)) {
		exit_proc(1);
	}
	if (verbose_mode) {
		fprintf(stderr,"file [%s] sent\n", s);
	}
}

void read_monitor_function(int i, double f, char *s)
{
	if (SFD < 0) {
		minimal_open();
	}
	if (verbose_mode) {
		bHashPrinting = true;
	}
	if (!read_ram(s, RAM_OFFS, iNeedbytes)) {
		exit_proc(2);
	}
}


void writeVar_function(int i, double f, char *s)
{
	// must "peek" before "poke" because we are using same address variable READ_VAR_ADDR
	bool result = false;
	if (abs(i) > 255) {
		result = monitor_poke_word(READ_VAR_ADDR, i);
	} else {
		result = monitor_poke_byte(READ_VAR_ADDR, (uint8_t)i);
	}
	if (!result) {
		fprintf(stderr, "missed poke 0x%02X to MEM[0x%04X]\n", i, READ_VAR_ADDR);
	}
	if (verbose_mode) {
		if (result) {
			fprintf(stderr, "poked MEM[0x%04X] := 0x%02X\n", READ_VAR_ADDR, i);
		}
	}
}


/*
void baud_function(int i, double f, char *s)
{
	CHECK_SERIAL_FD(__func__);

	serial_configure_ext(SFD, BAUD, iSerialBits, iSerialStop, iSerialParity, false);
}
*/

void baud_function(int i, double f, char *s)
{
	lateConfigure = true;
}

/*
void stop_function(int i, double f, char *s)
{
	lateConfigure = true;
	iSerialStop = 1;
}

void parity_function(int i, double f, char *s)
{
	lateConfigure = true;

	switch(*s) {
		case '0':
		case '1':
		case '2':
			iSerialParity = *s - '0';
			break;


		case 'm':
		case 'M':
		case 's':
		case 'S':
		case 'o':
		case 'O':
		case 'n':
		case 'N':
		case 'e':
		case 'E':
			iSerialParity = *s;
			break;
	}
}
*/


int last_signal = -1;
//int displayed_signal = -1;
typedef void (*sighandler_t)(int);
sighandler_t _main_old_handler;

void MySigHandler(int signum)
{
	switch(signum) {
		case SIGINT:
			exit_proc(0);
			break;

		case SIGCONT:
		case SIGHUP:
		case SIGPIPE:
		default:
	        last_signal = signum;
			break;
	}
}


int main(int argc, char *argv[])
{
	memset(_SKEY_BYTES, 0xFF, N_MAX_SECURITY_BYTES);

	signal(SIGINT, &(MySigHandler) );  // try to establish a exit handler
/*
	signal(SIGSTOP, &(MySigHandler) );
*/
	signal(SIGCONT, &(MySigHandler) );  	// wake-up after processor resume
	signal(SIGHUP, &(MySigHandler) );

	scan_args(argc, argv, opts);

	exit_proc(0, true);
}

