/********************************************************************** 
* HEADER_START                                                        * 
*                                                                     * 
*      Name:           HC08SPRG.H                                     * 
*      Project:        Developper's HC08 Bootloader Master            * 
*      Description:    Main header file                               * 
*      Platform:       Linux/Win32                                    * 
*      Revision:       1.0                                            * 
*      Date:           Apr 8 2002                                     * 
*      Author:         Michal Hanak                                   * 
*      Company:        Freescale                                      * 
*      Security:       General Business                               * 
*                                                                     * 
* =================================================================== * 
* Copyright (c):       FREESCALE Inc., 2004, All rights reserved.     * 
*                                                                     * 
* =================================================================== * 
* THIS SOFTWARE IS PROVIDED BY FREESCALE "AS IS" AND ANY              * 
* EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE * 
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR  * 
* PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL FREESCALE OR             * 
* ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,    * 
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT        * 
* NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;        * 
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)            * 
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, * 
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)       * 
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED * 
* OF THE POSSIBILITY OF SUCH DAMAGE.                                  * 
* =================================================================== * 
* 
* HEADER_END 
*/ 
 
#ifndef _HC08SPRG_H 
#define _HC08SPRG_H 
 
// types 
 
#define MAX_NUM_BLOCKS	16	// max. no. of Flash blocks 
#define MAX_SDID	32 		// max. SDID, if modified, complete the SDID table in prog.c 
#define SDID_UNDEF 0xFF		// HC08 (ver. 3) will report this (0xFF) 
#define MAX_ADDRESS  0x1000000 
#define MAX_LENGTH   0x100 
 
#define FAMILY_HC08    (1<<0) 
#define FAMILY_HCS08   (1<<1) 
#define FAMILY_NEXT    (1<<2) 
 
#define BL_HC08			(1 <<(FAMILY_HC08-1)) 
#define BL_HC08_LARGE	(BL_HC08 | 0x1 << FAMILY_HC08) 
 
#define BL_HCS08		(1 <<(FAMILY_HCS08-1)) 
#define BL_HCS08_LONG	(BL_HCS08 | 0x1 << FAMILY_HCS08) 
#define BL_HCS08_LARGE	(BL_HCS08 | 0x2 << FAMILY_HCS08) 
 
#define BL_UNKNOWN		-1 
 
 
typedef struct 
{ 
	unsigned mem_start[MAX_NUM_BLOCKS];	// start of usable flash
	unsigned mem_end[MAX_NUM_BLOCKS];	// end of usable flash
	unsigned erblk;			// erase block size 
	unsigned wrblk;			// write block size 
	unsigned bl_tbl;		// start of boot-loader table (private data and jump table) 
	unsigned int_vect_tbl;	// start of hard-wired interrupt vectors 
	BYTE priv_data[8];		// 8 bytes of private info 
	char targ_name[40];		// target name 
	unsigned num_blocks;	// number of flash memory blocks (BL protocol version 2 and up) 
	unsigned sdid;			// SDID number   (BL protocol version 2 and up) 
	unsigned sdidrev;		// SDID revision (BL protocol version 2 and up) 
} IDENT_DATA; 
 
typedef struct { 
	BYTE d[MAX_ADDRESS];	// data 
	BYTE f[MAX_ADDRESS];	// valid flag 0=empty; 1=usercode; 2=systemcode 
} BOARD_MEM; 
 
// globals 
 

extern IDENT_DATA ident; 
extern BOARD_MEM  image; 
extern int bl_version; 
extern int bl_rcs;	/* read command supported flag */ 
extern char * chipid; /* chip ID name */ 
extern int verify; 
 
// shared functions 
 
int read_s19(char *fn); 
int hook_reset(void); 
int calibrate_speed(void); 
int read_mcu_info(void); 
int setup_vect_tbl(void); 
int	prg_mem(void); 
int	check_image(void); 
int	unhook(void); 
 
#endif 


