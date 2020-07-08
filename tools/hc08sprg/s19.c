    /**********************************************************************  
    * HEADER_START                                                        *  
    *                                                                     *  
    *      Name:           S19.C                                          *  
    *      Project:        Developper's HC08 Bootloader Master            *  
    *      Description:    S19 manipulation routines file                 *  
    *      Platform:       Linux/Win32                                    *  
    *      Revision:       1.02 (also read S2/S3)                         *  
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
       
    ////////////////////////////////////////////////////////////////   
    // module vars   
       
    static char buf[512];   
       
    ///////////////////////////////////////////////////////////////   
    // read S19 file and store data into global memory image   
       
    int read_s19(char *fn)   
    {   
        FILE *f;   
        char c;   
        char* pc;   
        char afmt[7];   
        unsigned u, b, sum, len, alen;   
        unsigned long addr, total = 0, addr_lo = MAX_ADDRESS, addr_hi = 0;   
        int line = 0, terminate = 0;   
       
        if((f = *fn ? fopen(fn, "r") : stdin) == NULL)   
        {   
            fprintf(stderr, "Can't open input file %s\n", fn);   
            return -1;   
        }   
       
        // initialize g_image   
        memset(&image.d, 0xff, sizeof(image.d));   
        memset(&image.f, 0, sizeof(image.f));   
       
        while(!terminate && (pc = fgets(buf, sizeof(buf), f)) != NULL)   
        {   
            line++;   
       
            // S-records only   
            if(*pc++ != 'S')    
                continue;   
       
            // record type   
            switch(c = *pc++)   
            {   
            case '0':   // S0 is skipped   
                continue;   
            case '1':   // S1 is accepted   
                alen = 4;   
                break;   
            case '2':   // S2 is accepted   
                alen = 6;   
                break;   
            case '3':   // S3 is accepted   
                alen = 8;   
                break;   
            case '9':   // S9 terminates   
            case '8':   // S8 terminates   
            case '7':   // S7 terminates   
                terminate = 1;   
                continue;   
            default:    // others are reported and skipped   
                fprintf(stderr, "Skipping S%c record at line %d", c, line);   
                continue;   
            }   
       
            // prepare len & addr scan format   
            sprintf(afmt, "%%2x%%%dx", alen);   
       
            // scan len & address   
            if(sscanf(pc, afmt, &len, &addr)!=2 || len >= 0x100)   
            {   
                fprintf(stderr, "S-record parse error at line %d\n", line);   
                return -1;   
            }   
       
            if(addr >= MAX_ADDRESS)   
            {   
                fprintf(stderr, "Address out of range at line %d\n", line);   
                return -1;   
            }   
       
       
            printf("\rParsing S-record line %d ", line);   
            // skip len & address   
            pc += alen+2;   
           
            // init checksum   
            sum = len;   
            for(u=0; u<4; u++)   
                sum += (addr >> (u*8)) & 0xff;   
       
            // len & addr processed in record   
            len -= alen/2 + 1;   
       
            for(u=0; u<len; u++)   
            {
		int n = sscanf(pc, "%2x", &b);
                if(n != 1 || b >= 0x100)   
                {   
                    fprintf(stderr, "S-record data parse error at line %d, %04X [%s] n:%d\n", line, b, pc, n); 
                    return -1;   
                }   
                pc += 2;   
       
                image.d[addr+u] = b;   
                image.f[addr+u] = 1;   
                sum += b;   
                total++;   
       
                if (addr+u < addr_lo)   
                    addr_lo = addr+u;   
                if (addr+u > addr_hi)   
                    addr_hi = addr+u;   
            }   
       
            printf("Bytes: %d", total);   
       
            // test CS   
            if(sscanf(pc, "%2x", &b) != 1 || b >= 0x100 || ((sum+b)&0xff) != 0xff)   
            {   
                fprintf(stderr, "S-record checksum error at line %d, %04X, %02X\n", line, sum, b);   
                return -1;   
            }   
        }   
       
        if (total == 0)   
        {   
            fprintf(stderr, "S-record contains no valid data!\n");   
            return -1;   
        }   
        else   
        {   
            printf("\rParsed S-record lines: %d  Bytes total: %d\n", line, total);   
            if (addr_hi >= 0x10000)   
                printf("Source address range: 0x%06X-0x%06X\n\n", addr_lo, addr_hi);   
            else   
                printf("Source address range: 0x%04X-0x%04X\n\n", addr_lo, addr_hi);   
        }   
        fclose(f);   
        return 0;   
    }   
       


