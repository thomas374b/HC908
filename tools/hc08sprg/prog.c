    /**********************************************************************  
    * HEADER_START                                                        *  
    *                                                                     *  
    *      Name:           PROG.C                                         *  
    *      Project:        Developper's HC08 Bootloader Master            *  
    *      Description:    Main programming routines file                 *  
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
       
    #include "sysdep.h"   
    #include "hc08sprg.h"   
    #include "serial.h"   
       

    /////////////////////////////////////////////////   
    // bootloader commands   
       
    #define WR_DATA     'W'   
    #define RD_DATA     'R'   
    #define IDENT       'I'   
    #define QUIT        'Q'   
    #define ERASE       'E'   
    #define ACK         0xFC   
       
    char *SDID[MAX_SDID]=   
    {   
        "MC9S08CT16",                       //0  (never released, experimental only)   
        "unknown",                          //1   
        "MC9S08(GB/GT)(32-60),MC1321x",     //2   
        "MC9S08(RC/RD/RE)(8/16)",           //3   
        "MC9S08(RC/RD/RE/RG)(32-60)",       //4   
        "unknown",                          //5   
        "MC3S08RX32",                       //6   
        "MC3S08RE16",                       //7   
        "MC9S08AW(16-60)",                  //8   
        "MC9S08QG(4/8)",                    //9   
        "MC9S08JR12",                       //10 (0x0A)   
        "unknown",                          //11 (0x0B)   
        "MC9S08LC(36-60)",                  //12 (0x0C)   
        "unknown",                          //13 (0x0D)   
        "MC9S08(DE/DN/DZ/DV/EN)(16-60)",    //14 (0x0E)   
        "unknown",                          //15 (0x0F)   
        "MC9S08RA16",                       //16 (0x10)   
        "MC9S08QD(4/8)",                    //17 (0x11)   
        "unknown",                          //18 (0x12)   
        "MC9S08(EL/SL)(8/16/32)",           //19 (0x13)   
        "MC9S08(SG/SH)(4/8)",               //20 (0x14)   
        "MC9S08QE(64/96/128)",              //21 (0x15)   
        "MC9S08JM(16-60)",                  //22 (0x16)   
        "unknown",                          //23 (0x17)   
        "unknown",                          //24 (0x18)   
        "MC9S08(DZ/DV)(96/128)",            //25 (0x19)   
        "MC9S08(SG/SH)(16/32)",             //26 (0x1A)   
        "unknown",                          //27 (0x1B)   
        "unknown",                          //28 (0x1C)   
        "unknown",                          //29 (0x1D)   
        "unknown",                          //30 (0x1E)   
        "unknown",                          //31 (0x1F)
/*
        "MC68HC908JK1",                     //32 (0x20)
        "MC68HC908JK3",                     //33 (0x21)
        "MC68HC908JB8",                     //34 (0x22)
        "MC68HC908MR32CFU",                 //35 (0x23)
*/
    };   
       
    char SDID2blver[MAX_SDID]=   
    {   
        BL_HCS08,           //0     "MC9S08CT16",                       //0  (never released, experimental only))   
        BL_UNKNOWN,         //1     "unknown",                          //1   
        BL_HCS08,           //2     "MC9S08(GB/GT)(32-60),MC1321x",     //2   
        BL_HCS08,           //3     "MC9S08(RC/RD/RE)(8/16)",           //3   
        BL_HCS08,           //4     "MC9S08(RC/RD/RE/RG)(32-60)",       //4   
        BL_UNKNOWN,         //5     "unknown",                          //5   
        BL_HCS08,           //6     "MC3S08RX32",                       //6   
        BL_HCS08,           //7     "MC3S08RE16",                       //7   
        BL_HCS08,           //8     "MC9S08AW(16-60)",                  //8   
        BL_HCS08,           //9     "MC9S08QG(4/8)",                    //9   
        BL_HCS08,           //10    "MC9S08JR12",                       //10 (0x0A)   
        BL_UNKNOWN,         //11    "unknown",                          //11 (0x0B)   
        BL_HCS08,           //12    "MC9S08LC(36-60)",                  //12 (0x0C)   
        BL_UNKNOWN,         //13    "unknown",                          //13 (0x0D)   
        BL_HCS08_LONG,      //14    "MC9S08(DE/DN/DZ/DV/EN)(16-60)",    //14 (0x0E)   
        BL_UNKNOWN,         //15    "unknown",                          //15 (0x0F)   
        BL_HCS08,           //16    "MC9S08RA16",                       //16 (0x10)   
        BL_HCS08,           //17    "MC9S08QD(4/8)",                    //17 (0x11)   
        BL_UNKNOWN,         //18    "unknown",                          //18 (0x12)   
        BL_HCS08_LONG,      //19    "MC9S08(EL/SL)(8/16/32)",           //19 (0x13)   
        BL_HCS08,           //20    "MC9S08(SG/SH)(4/8)",               //20 (0x14)   
        BL_HCS08_LARGE,     //21    "MC9S08QE(64/96/128)",              //21 (0x15)   
        BL_HCS08,           //22    "MC9S08JM(16-60)",                  //22 (0x16)   
        BL_UNKNOWN,         //23    "unknown",                          //23 (0x17)   
        BL_UNKNOWN,         //24    "unknown",                          //24 (0x18)   
        BL_HCS08_LONG,      //25    "MC9S08(DZ/DV)(96/128)",            //25 (0x19)   
        BL_HCS08,           //26    "MC9S08(SG/SH)(16/32)",             //26 (0x1A)   
        BL_UNKNOWN,         //27    "unknown",                          //27 (0x1B)   
        BL_UNKNOWN,         //28    "unknown",                          //28 (0x1C)   
        BL_UNKNOWN,         //29    "unknown",                          //29 (0x1D)   
        BL_UNKNOWN,         //30    "unknown",                          //30 (0x1E)   
        BL_UNKNOWN          //31    "unknown",                          //31 (0x1F)
/*
        BL_HC08          	//32    "unknown",                          //31 (0x20)
        BL_HC08          	//33    "unknown",                          //31 (0x21)
        BL_HC08          	//34    "unknown",                          //31 (0x22)
        BL_HC08		        //35    "unknown",                          //31 (0x23)
*/
    };   
       
    /////////////////////////////////////////////////////////////////   
    // helper compare of unsigned    
       
    static unsigned umin(unsigned a, unsigned b)   
    {   
        return a < b ? a : b;   
    }   
       
    //////////////////////////////////////////////////////////////////   
    // helper read 16 bit number   
    // return -1=error; 0=rd timeout; 2=success (2 bytes read)   
       
    static int r16(unsigned * n)
    {   
        int ret;   
        BYTE b[2];   
       
        // we need 2 bytes (otherwise return -1 or 0)   
        if((ret=rb(b, 2)) != 2)    
            return ret < 0 ? ret : 0;   
       
        // first was HI, then LO   
        *n = b[0] << 8;   
        *n += b[1];   
        return 2;   
    }   
       
    /////////////////////////////////////////////////////////////////////   
    // helper write 16 bit number   
    // return -1=error; 0=wr timeout; 2=success (2 bytes written)   
       
    static int w16(unsigned n)   
    {   
        int ret;   
        BYTE b[2];   
       
        b[0] = (BYTE) ((n & 0xff00) >> 8);   
        b[1] = (BYTE) (n & 0xff);   
        if((ret=wb(b, 2)) != 2)    
            return ret < 0 ? -1 : 0;   
       
        return 2;   
    }   
       
       
    //////////////////////////////////////////////////////////////////   
    // helper read 24 bit number   
    // return -1=error; 0=rd timeout; 3=success (3 bytes read)   
       
    static int r24(unsigned * n)
    {   
        int ret;   
        BYTE b[3];   
       
        // we need 3 bytes (otherwise return -1 or 0)   
        if((ret=rb(b, 3)) != 3)    
            return ret < 0 ? ret : 0;   
       
        // first was HI, then LO   
        *n =  b[0] << 16;   
        *n += b[1] << 8;   
        *n += b[2];   
        return 3;   
    }   
       
    /////////////////////////////////////////////////////////////////////   
    // helper write 24 bit number   
    // return -1=error; 0=wr timeout; 3=success (3 bytes written)   
       
    static int w24(unsigned long n)   
    {   
        int ret;   
        BYTE b[3];   
       
        b[0] = (BYTE) ((n & 0xff0000) >> 16);   
        b[1] = (BYTE) ((n & 0xff00) >> 8);   
        b[2] = (BYTE) (n & 0xff);   
        if((ret=wb(b, 3)) != 3)    
            return ret < 0 ? -1 : 0;   
       
        return 3;   
    }   
       
    //////////////////////////////////////////////////////////////   
    // wait for hc08 reset   
       
    // target send us ACK (0xfc) but in this time, it might be off-key up to factor 3   
    // so we might receive FF, FE, FC, F8, ..., 80, 0   
       
    static int could_be_ack(unsigned b)   
    {   
        b |= 0xff00;   
        while(!(b&1))   
            b = b >> 1;   
       
        return (b & 0xff) == 0xff ? 1 : 0;   
    }   
       
    int hook_reset(void)   
    {   
        int ret;   
        BYTE d;   
       
        printf("Waiting for HC(S)08 reset ACK...");   
        fflush(stdout);   
       
        // ignore timeouts   
        while((ret=rb(&d, 1)) >= 0)   
        {   
            if(ret > 0)   
            {   
                printf("received 0x%02x ", d);   
                if(could_be_ack(d))   
                {   
                    if(d == ACK) printf("(good).\n");   
                    else printf("(off-key).\n");   
                    break;   
                }   
                else   
                {   
                    printf("(ignoring).\n");   
                }   
            }   
        }   
       
        if(ret < 0)   
        {   
            printf("\n");   
            fprintf(stderr, "Error receiving reset ACK.\n");   
            return -1;          
        }   
       
        // enter interactive mode   
        if(wb(&d, 1) <= 0)   
        {   
            fprintf(stderr, "Error sending locking ACK.\n");   
            return -1;          
        }   
       
        // wait for finish   
        flush_uart(FDRAIN, 0);   
       
        // success   
        return 1;   
    }   
       
    ///////////////////////////////////////////////////////////////   
    // send breaks to calibrate target's oscillator   
       
    int calibrate_speed(void)   
    {   
        BYTE b;   
        int ret, cnt=0;   
       
        for(;;)   
        {   
            // wait for ACK or timeout   
            ret = rb(&b, 1);   
            // read error   
            if(ret < 0)   
            {   
                if(cnt) printf("\n");   
                fprintf(stderr, "Error receiving calibrate ACK.\n");   
                return -1;   
            }   
            // got something   
            else if(ret > 0)   
            {   
                // it is wrong to receive anything other than ACK   
                if(b != ACK)   
                {   
                    if(cnt) printf("\n");   
                    fprintf(stderr, "Wrong calibrate response (0x%02x, must be 0x%02x). Target is not calibrated.\n", b, ACK);   
                    return -1;   
                }   
                // OK, exit calibration loop   
                break;   
            }   
               
            // rx timeout ! send 1.04ms break pulse (again)   
            if(send_break10() < 0)    
            {   
                if(cnt) printf("\n");   
                fprintf(stderr, "Can't send break pulse.\n");   
                return -1;   
            }   
       
            // another break sent   
            printf("\rCalibration break pulse sent. Count: %d ", ++cnt);   
            fflush(stdout);   
        };   
       
        if(cnt) printf("\n");   
        // success (got ACK)   
        return 1;   
    }   
       
       
    ///////////////////////////////////////////////////////////////   
    // simple return-on-error macros   
       
    #define R8(var)     {if(rb(var, 1) != 1) return -1;}   
    #define R16(var)    {if(r16(var) <= 0) return -1;}   
    #define R24(var)    {if(r24(var) <= 0) return -1;}   
    #define W8(n)       { BYTE _x=n; if(wb(&_x, 1) != 1) return -1; }   
    #define W16(n)      {if(w16(n) <= 0) return -1;}   
    #define W24(n)      {if(w24(n) <= 0) return -1;}   
    //////////////////////////////////////////////////////////////   
    // get basic info about target board   
       
    int read_mcu_info(void)   
    {   
        BYTE d;   
        unsigned i;   
       
        // initialize memory space   
        memset(&ident, 0, sizeof(ident));   
       
        // issue command   
        flush_uart(FDISCARD, FDISCARD);   
        W8(IDENT);   
       
        // first byte is protocol version   
        R8(&d);    
        bl_version = (int) (d & 0x0F);   
        bl_rcs = (((int)d & 0x80) != 0);   
       
        if(bl_version == BL_HC08)   
        {   
            // read identification structure for version 1 (HC08)   
            ident.num_blocks = 1;   
            R16(&ident.mem_start[0]);   
            R16(&ident.mem_end[0]);   
            R16(&ident.bl_tbl);   
            R16(&ident.int_vect_tbl);   
            R16(&ident.erblk);   
            R16(&ident.wrblk);   
            // read private data   
            if(rb(&ident.priv_data, sizeof(ident.priv_data)) != sizeof(ident.priv_data))   
                return -1;   
        }    
        else if((bl_version == BL_HCS08)    
             || (bl_version == BL_HC08_LARGE)    
             || (bl_version == BL_HCS08_LONG)    
             || (bl_version == BL_HCS08_LARGE))   
        {   
            unsigned i, sdidh, sdidl;   
            // read identification structure for version 2 (HCS08), 3 (large HC08), 6 (HCS08 with "non-binary" Erase/Write size)   
            // 10 (HCS08 with >64kB memory), 12 (long HCS08)   
            R8(&sdidh);   
            R8(&sdidl);   
            ident.sdid = ((sdidh & 0x0F) << 8) | (sdidl & 0xFF);   
            ident.sdidrev = ((sdidh & 0xF0) >> 4);   
            R8(&ident.num_blocks);   
            if ((ident.num_blocks > MAX_NUM_BLOCKS) || (ident.num_blocks == 0))   
                return -1;   
            for(i=0; i<ident.num_blocks; i++)   
            {   
                if((bl_version == BL_HCS08_LARGE) || (bl_version == BL_HCS08_LONG)) // versions 10 + 12 (HCS08 with >64kB memory) is 24-bit address here   
                {   
                    R24(&ident.mem_start[i]);   
                    R24(&ident.mem_end[i]);   
                }   
                else   
                {   
                    R16(&ident.mem_start[i]);   
                    R16(&ident.mem_end[i]);   
                }   
            }   
            R16(&ident.bl_tbl);   
            R16(&ident.int_vect_tbl);   
            R16(&ident.erblk);   
            R16(&ident.wrblk);   
       
            if (bl_version != BL_HC08_LARGE)    // check only S08 families, not HC08   
            {   
                if(ident.sdid >= MAX_SDID)   
                {      
                    fprintf(stderr, "\nERROR! The SDID of the device [0x%03X] is too high and not (yet) supported! Check AN2295SW for update!\n", ident.sdid);   
                    return -1;   
                }   
                else   
                {   
                    chipid = SDID[ident.sdid];   
                    if (SDID2blver[ident.sdid] == BL_UNKNOWN)   
                    {   
                        fprintf(stderr, "\nWARNING! The SDID of the device [0x%03X] is not (yet) supported! Check AN2295SW for update!\n", ident.sdid);   
                    }   
                    else   
                    {   
                        if (SDID2blver[ident.sdid] != bl_version)   
                            fprintf(stderr, "\nWARNING! The protocol version [0x%02X] does not match the one associated\n"   
                                              "with the device with following SDID [0x%03X].\n"   
                                              "Continuing with reported protocol version anyway.\n\n"   
                                              "Something is screwed up, please report!\n", bl_version, ident.sdid);   
                    }   
                }   
            }   
        }   
        else   
            return -1;   
       
       
        // read string   
        i = 0;   
        // read all name characters   
        do   
        {   
            R8(&d);   
       
            // but store up to our buffer len chars only   
            if(i < sizeof(ident.targ_name)-1)   
                ident.targ_name[i++] = d;   
       
        // zero terminated   
        } while(d);   
       
        return 0;   
    }   
       
    //////////////////////////////////////////////////////////////   
    // read memory block, return number of bytes read or -1   
       
    int read_blk(unsigned a, int len, BYTE *dest)   
    {   
        if(len < 0 || len >= MAX_LENGTH || a >= MAX_ADDRESS)   
            return -1;   
       
        // issue command   
        flush_uart(FDISCARD, FDISCARD);   
        W8(RD_DATA);   
        // and parameters   
        if ((bl_version == BL_HCS08_LARGE) || (bl_version == BL_HCS08_LONG)) W24(a) else W16(a);   
        W8(len);   
       
        // block read   
        if(rb(dest, len) != len)   
            return -1;   
       
        // return number of bytes read (==requested)   
        return len;   
    }   
       
    //////////////////////////////////////////////////////////////   
    // masquerade vector table (work on memory image)   
    // the trick is to move any vector values from S19 file to    
    // special jump table located at ident.app_vect_tbl   
       
    int setup_vect_tbl(void)   
    {   
        unsigned src, dest;   
        int any_hit = 0;   
       
        if((bl_version == BL_HC08) || (bl_version == BL_HC08_LARGE))   
        {   
            // start of jump table   
            dest = ident.bl_tbl;   
       
            // we store private data first   
            memcpy(&image.d[dest], ident.priv_data, sizeof(ident.priv_data));   
            dest += sizeof(ident.priv_data);   
       
            // and jump-table next   
            for(src = 0xfffe; src >= ident.int_vect_tbl; src -= 2)   
            {   
            	if ((src+1 != 0xFFFF) && (image.d[src] == 0xFF) && (image.d[src+1] == 0xFF)) {
            		// appplication has an empty or invalid vector
                    image.d[dest++] = 0x80; // RTI instruction
                    image.d[dest++] = 0x9D;
                    image.d[dest++] = 0x9D;
            	} else {
            		image.d[dest++] = 0xcc; // jump instruction
            		image.d[dest++] = image.d[src];
            		image.d[dest++] = image.d[src+1];
            	}
                // is the vector valid ?   
                if(image.f[src] && image.f[src+1])   
                    any_hit = 1;   
       
                // original vector (from s19 image) is no longer valid   
                image.f[src] = 0;   
                image.f[src+1] = 0;   
            }   
        }   
        else if ((bl_version == BL_HCS08) || (bl_version == BL_HCS08_LONG))   
        {   
            // start of jump table   
            dest = ident.bl_tbl;   
       
            // private data not stored (YET)   
            // and jump-table next   
            for(src = ident.int_vect_tbl; src <= 0xfffe ; src += 2)   
            {   
                image.d[dest++] = image.d[src];    
                image.d[dest++] = image.d[src+1];   
       
                // is the vector valid ?   
                if(image.f[src] && image.f[src+1])   
                    any_hit = 1;   
       
                // original vector (from s19 image) is no longer valid   
                image.f[src] = 0;   
                image.f[src+1] = 0;   
            }   
        }      
        else if (bl_version == BL_HCS08_LARGE)  //at least QE family does not support the redirection, so use 'old' style   
        {   
            // start of jump table   
            dest = ident.bl_tbl;   
       
            // and jump-table next   
            for(src = 0xfffe; src >= ident.int_vect_tbl; src -= 2)   
            {   
                image.d[dest++] = 0xcc; // jump instruction   
                image.d[dest++] = image.d[src];    
                image.d[dest++] = image.d[src+1];   
       
                // is the vector valid ?   
                if(image.f[src] && image.f[src+1])   
                    any_hit = 1;   
       
                // original vector (from s19 image) is no longer valid   
                image.f[src] = 0;   
                image.f[src+1] = 0;   
            }   
        }   
       
       
        // write the whole system area, but only if at least one vector was valid in the S19   
        if(any_hit)   
            memset(&image.f[ident.bl_tbl], 2, dest - ident.bl_tbl);   
       
        return 0;   
    }   
       
    //////////////////////////////////////////////////////////////   
    // erase single block around given address   
       
    int erase_blk(unsigned /*long */a)   
    {   
        int ret;   
        BYTE d;   
       
        DBG1("\nerase_blk: 0x%06x\n", a);   
        if(a >= MAX_ADDRESS)   
            return -1;   
       
        // issue command   
        flush_uart(FDISCARD, FDISCARD);   
        W8(ERASE);   
        // and parameter   
        if ((bl_version == BL_HCS08_LARGE) || (bl_version == BL_HCS08_LONG)) W24(a) else W16(a);   
       
        // wait for ack   
        while((ret = rb(&d, 1)) == 0)   
            ; // do nothing   
       
        // it must be ACK character   
        if(ret > 0 && d != ACK)   
            ret = -1;   
       
        // either -1 or 1   
        return ret;   
    }   
       
    //////////////////////////////////////////////////////////////   
    // program single block   
       
    int prg_blk(unsigned long a, int len)   
    {   
        int ret;   
        BYTE d;   
       
        DBG2("\nprg_blk: 0x%06x-0x%06x\n", a, a+len-1);   
        if(len < 0 || len >= MAX_LENGTH || a >= MAX_ADDRESS)    
            return -1;   
       
        // issue command   
        flush_uart(FDISCARD, FDISCARD);   
        W8(WR_DATA);   
        // and parameters   
        if ((bl_version == BL_HCS08_LARGE) || (bl_version == BL_HCS08_LONG)) W24(a) else W16(a);   
        W8(len);   
       
        if(wb(&image.d[a], len) != len)   
            return -1;   
       
        // wait for ack   
        while((ret = rb(&d, 1)) == 0)   
            ; // do nothing   
       
        // must be ACK character   
        if(ret > 0 && d != ACK)   
            ret = -1;   
       
        // either -1 or 1   
        return ret;   
    }   
       
    //////////////////////////////////////////////////////////////   
    // program one continuous area and give user a progess    
       
    static int prg_area(unsigned long start, unsigned long end)   
    {   
        unsigned long i, er, wr, er_next, wr_end, wr_next;   
        unsigned long wr_one, written=0, total=0;   
       
        DBG2("prg_area: 0x%06x-0x%06x\n", start, end);   
        if(start >= MAX_ADDRESS || end >= MAX_ADDRESS)    
            return -1;   
       
        // count valid bytes   
        for(i=start; i<end; i++)   
            if(image.f[i]) total++;   
       
       
    #define FLASHMODULO(x,y) ((unsigned long)(MAX_ADDRESS - (y) *(1 + ((MAX_ADDRESS-(unsigned long)((x)+(y))-1) / (y)))))   
       
       
        // take start address as it is, but do further steps to erblk boundaries   
        for(er=start; er<end; er=er_next)   
        {   
            // start of next erase block   
            er_next = FLASHMODULO(er,ident.erblk);   
       
            // anything to program in this erase block ?   
            wr = wr_end = er;   
            for(i=er; i<er_next; i++)   
            {   
                // valid byte   
                if(image.f[i])   
                {   
                    if(!image.f[wr]) wr = i;   
                    wr_end = i+1;   
                }   
            }   
       
            // never pass after end   
            if(wr_end > end)   
                wr_end = end;   
       
            // wr is now pointing to first valid byte (within current erase block)   
            // wr_end is now pointing after last valid byte (within current erase block)   
            if(wr < wr_end)   
            {   
                if ((bl_version == BL_HCS08_LARGE) || (bl_version == BL_HCS08_LONG))    
                    printf("\rMemory programming: E 0x%06X %d%%", wr, 100*written/total);   
                else   
                    printf("\rMemory programming: E 0x%04X %d%%", wr, 100*written/total);   
                fflush(stdout); DBG("\n");   
       
                // use the first valid-byte address   
                if(erase_blk(wr) < 0)    
                {   
                    printf("\n");   
                    if ((bl_version == BL_HCS08_LARGE) || (bl_version == BL_HCS08_LONG))    
                        fprintf(stderr, "Can't erase block at address 0x%06X\n", wr);   
                    else       
                        fprintf(stderr, "Can't erase block at address 0x%04X\n", wr);   
                    return -1;   
                }   
       
                for(/* original wr */; wr<wr_end; wr=wr_next)   
                {   
                    // start of next write block   
                    wr_next = FLASHMODULO(wr,ident.wrblk);   
       
                    if ((bl_version == BL_HCS08_LARGE) || (bl_version == BL_HCS08_LONG))    
                        printf("\rMemory programming: W 0x%06X %d%%", wr, 100*written/total);   
                    else   
                        printf("\rMemory programming: W 0x%04X %d%%", wr, 100*written/total);   
       
                    fflush(stdout); DBG("\n");   
       
                    wr_one = umin(wr_end, wr_next)-wr;   
                    if(prg_blk(wr, wr_one) < 0)    
                    {   
                        printf("\n");   
                        if ((bl_version == BL_HCS08_LARGE) || (bl_version == BL_HCS08_LONG))    
                            fprintf(stderr, "Can't program block at address 0x%06X\n", wr);   
                        else   
                            fprintf(stderr, "Can't program block at address 0x%04X\n", wr);   
                        return -1;   
                    }   
       
                    if((bl_rcs) && (verify))    // read command implemented!   
                    {   
                        unsigned char rbuff[256];   
                        unsigned int adr;   
       
                        if ((bl_version == BL_HCS08_LARGE) || (bl_version == BL_HCS08_LONG))    
                            printf("\rMemory reading:     R 0x%06X %d%%", wr, 100*written/total);   
                        else   
                            printf("\rMemory reading:     R 0x%04X %d%%", wr, 100*written/total);   
       
                        fflush(stdout); DBG("\n");   
                        if (read_blk(wr, wr_one, rbuff) <0)   
                        {   
                            printf("\n");   
                            if ((bl_version == BL_HCS08_LARGE) || (bl_version == BL_HCS08_LONG))    
                                fprintf(stderr, "Can't read block at address 0x%06X\n", wr);   
                            else   
                                fprintf(stderr, "Can't read block at address 0x%04X\n", wr);   
                            return -1;   
                        }   
       
                        for(adr = wr; adr < wr+wr_one; adr++)   
                        {   
                            if (image.d[adr] != rbuff[adr-wr])   
                            {   
                                if ((bl_version == BL_HCS08_LARGE) || (bl_version == BL_HCS08_LONG))    
                                    fprintf(stderr, "\nVerification failed at address 0x%06X, image: [0x%02X], MCU: [0x%02X]\n", adr, image.d[adr], rbuff[adr-wr]);   
                                else   
                                    fprintf(stderr, "\nVerification failed at address 0x%04X, image: [0x%02X], MCU: [0x%02X]\n", adr, image.d[adr], rbuff[adr-wr]);   
                                return -1;   
                            }   
                        }   
                    }   
       
                    // the percentage-counting algorithm is not perfect, in some cases there might    
                    // be more than 100% achieved (if S19 file has holes within erblks = rare case)   
                    if((written += wr_one) > total)   
                        written = total;   
                }   
            }   
        }   
       
        return 0;   
    }   
       
    //////////////////////////////////////////////////////////////   
    // program all   
       
    int prg_mem(void)   
    {   
        unsigned s,e;   
       
        // find first and last valid byte   
        for(s=0; s<MAX_ADDRESS; s++)   
            if(image.f[s]) break;   
        for(e=MAX_ADDRESS; e>s; e--)   
            if(image.f[e-1]) break;   
       
        if(prg_area(s, e) < 0)   
            return -1;   
       
       
        if ((bl_version == BL_HCS08_LARGE) || (bl_version == BL_HCS08_LONG))    
        {   
            printf("\rMemory programmed:             100%%\n");   
            if((bl_rcs) && (verify))    
                printf("\rMemory verified:               OK\n");   
        }   
        else   
        {   
            printf("\rMemory programmed:           100%%\n");   
            if((bl_rcs) && (verify))    
                printf("\rMemory verified:             OK\n");   
        }   
        return 0;   
    }   
       
    //////////////////////////////////////////////////////////////   
    // print ID$, only printable char, max. 255 chars   
       
    void cleanprint(char * str)   
    {   
    int cnt = 255;   
           
        while(*str && cnt--)    
        {   
            if (isprint(*str))   
                putchar(*str);   
            else   
                putchar('.');   
       
            str++;   
        }   
    }   
       
    //////////////////////////////////////////////////////////////   
    // check if code goes to valid memory only   
       
    int check_image()   
    {   
        unsigned a, i, f;   
       
        unsigned s,e;   
       
        char* scannm;   
        char id;   
       
       
        // find first and last valid byte   
        for(s=0; s<MAX_ADDRESS; s++)   
            if(image.f[s]) break;   
        for(e=MAX_ADDRESS; e>s; e--)   
            if(image.f[e-1]) break;   
       
       
        // skip COM if specified and scan number and mode (optional)   
        id = strncmp((char*)image.d+s, "ID$:", 4);
       
        if (id == 0)   
        {   
            scannm = (char*)(image.d+s+4);   /* string ptr. after ID$ */
       
            printf("\r\nMCU software version: ");   
            cleanprint(scannm);   
            putchar('\n');   
        }   
       
        //sscanf(scannm, "%i%c%c", &portNum, &modemode, &serial_mode);   
       
           
           
        for(a=0; a<MAX_ADDRESS; a++)   
        {   
            if(image.f[a]==1)   
            {   
                f = 0;   
                for(i=0; i<ident.num_blocks; i++)   
                {   
                    if ((a>=ident.mem_start[i]) && (a<ident.mem_end[i]))   
                    {   
                        f = 1;   
                        break;   
                    }   
                }   
                if(f == 0)   
                    goto err_hit;   //location at address a won't fit any memory block!   
            }   
        }   
       
        // no error   
        return 0;   
       
    err_hit:   
        if ((bl_version == BL_HCS08_LARGE) || (bl_version == BL_HCS08_LONG))    
            fprintf(stderr, "\nWARNING! S19 image will not fit into available memory (at address 0x%06X)!\n", a);   
        else   
            fprintf(stderr, "\nWARNING! S19 image will not fit into available memory (at address 0x%04X)!\n", a);   
        return -1;   
    }   
       
    //////////////////////////////////////////////////////////////   
    // exit program mode, start application   
       
    int unhook(void)   
    {   
        W8(QUIT);   
        return 0;   
    }   
       


