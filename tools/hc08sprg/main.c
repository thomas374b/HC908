    /**********************************************************************  
    * HEADER_START                                                        *  
    *                                                                     *  
    *      Name:           MAIN.C                                         *  
    *      Project:        Developper's HC08 Bootloader Master            *  
    *      Description:    MAIN file                                      *  
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
       
    /*  
        Changelog:  
            30-May-2008     MC9S08(SG/SH)(16/32) family added (SDID 0x1A)  
            12-Jun-2008     MC9S08(DZ/DV)(96/128) family added (SDID 0x19), BL_HCS08_LONG  
      
    */   
       
#include <ctype.h>
       
    #include "sysdep.h"   
    #include "hc08sprg.h"   
    #include "serial.h"   
       
    ////////////////////////////////////////////////   
    // global vars   
       
    IDENT_DATA ident;   
    BOARD_MEM  image;   
    int bl_version;   
    char * chipid;   
    int bl_rcs; /* read command supported flag */   
    char defaultyes = 0;   
       
    ////////////////////////////////////////////////   
       
    char * Version_string(int bl_ver)   
    {   
        int i=0;   
       
        char bl_ver_index[]=   
        {   
            BL_HC08, BL_HC08_LARGE, BL_HCS08, BL_HCS08_LONG, BL_HCS08_LARGE, 0   
        };   
       
        char *BL_VER_STRING[]=   
        {   
            "HC08",         //1   
            "large HC08",   //3   
            "S08",          //2   
            "long S08",     //6   
            "large S08",    //10   
            "unknown",      //0  protocol version 0 does not exist, also needs to be a last one = zero   
        };   
       
        while(bl_ver_index[i])   
        {   
            if(bl_ver_index[i] == bl_ver)   // find the friendly name of the protocol   
                break;   
            i++;   
        };   
       
        return BL_VER_STRING[i];   
    }   
       
    int main(int argc, char *argv[ ])   
    {   
        printf("---------------------------------------------------------------------------\n");   
        printf("hc08sprg - Developer's Serial Bootloader for HC(S)08 - $Version: 9.0.41.0$ \n");   
        printf("---------------------------------------------------------------------------\n");   
        printf("FC protocol versions supported:\n"   
            "\t0x%02X (%s)\n"   
            "\t0x%02X (%s)\n"   
            "\t0x%02X (%s)\n"   
            "\t0x%02X (%s)\n"    
            "\t0x%02X (%s)\n",    
            BL_HC08,            Version_string(BL_HC08),   
            BL_HC08_LARGE,      Version_string(BL_HC08_LARGE),   
            BL_HCS08,           Version_string(BL_HCS08),   
            BL_HCS08_LONG,      Version_string(BL_HCS08_LONG),   
            BL_HCS08_LARGE,     Version_string(BL_HCS08_LARGE));   
        printf("---------------------------------------------------------------------------\n\n");   
       
        if(argc != 3 && argc != 4)   
        {   
            printf(   
                "usage:  hc08sprg port[:|!][D|d|S|s|?] [speed] file\n"   
                "        port:D ... dual wire mode [default]\n"   
                "        port:d ... dual wire mode with verification supressed\n"   
                "        port:S ... single wire mode\n"   
                "        port:s ... single wire mode with verification supressed\n"   
                "        port:? ... detect single/dual wire mode (use with care)\n"   
                "            !  ... batch mode, no questions\n"   
                "        speed  ... speed in bps\n"   
                "        file   ... S19 file\n\n"   
                "See Freescale Application Note AN2295 and AN2295SW for updates.\n");   
            exit(0);   
        }   
       
        // open serial port (speed specified ?)   
        if(init_uart(argv[1], argc==4 ? argv[2] : NULL) < 0)   
            exit(1);   
       
        // read S19 file   
        if(read_s19(argv[argc-1]) < 0)   
            exit(2);   
       
        // wait for reset   
        if(hook_reset() < 0)    
            exit(3);   
       
        // calibrate target's oscilator   
        if(calibrate_speed() < 0)   
            exit(4);   
       
        // read info   
        if(!read_mcu_info())   
        {   
            printf("\nBootloader protocol version: 0x%02X (%s, ", bl_version /*| (bl_rcs ? 0x80 : 0)*/, Version_string(bl_version));   
            printf("read command ");   
            if (!bl_rcs) printf("NOT ");   
            printf("supported)\n");   
            printf("Bootloader version string: %s\n", ident.targ_name);   
       
            if (bl_version == BL_HC08) // HC08   
            {   
                unsigned i;   
                printf("Available flash memory: 0x%04X-0x%04X\n", ident.mem_start[0], ident.mem_end[0]-1);   
                printf("Erase block size: %3d bytes\n", ident.erblk);   
                printf("Write block size: %3d bytes\n", ident.wrblk);   
                printf("Original vector table: 0x%04X\n",ident.int_vect_tbl);   
                printf("Bootloader user table: 0x%04X\n",ident.bl_tbl);   
                printf("Bootloader data (hex): ");   
                for(i=0; i<sizeof(ident.priv_data); i++)   
                    printf("%02x ", ident.priv_data[i]);   
                printf("\n");   
            }   
            else if (bl_version == BL_HCS08) // S08   
            {   
                unsigned i;   
                printf("System device ID: 0x%03X [%s] rev. %u\n", ident.sdid, chipid ,ident.sdidrev);   
                printf("Number of memory blocks: %u\n", ident.num_blocks);   
                for(i=0; i<ident.num_blocks; i++)   
                    printf("Memory block #%u: 0x%04X-0x%04X\n", i+1, ident.mem_start[i], ident.mem_end[i]-1);   
                printf("Erase block size: %3d bytes\n", ident.erblk);   
                printf("Write block size: %3d bytes\n", ident.wrblk);   
                printf("Original vector table: 0x%04X-0xFFFF\n",ident.int_vect_tbl);   
                printf("New vector table:      0x%04X-0x%04X\n",ident.bl_tbl, ident.bl_tbl+(0xFFFF-ident.int_vect_tbl));   
            }   
            else if (bl_version == BL_HC08_LARGE)   // large HC08   
            {   
                unsigned i;   
                printf("Number of memory blocks: %u\n", ident.num_blocks);   
                for(i=0; i<ident.num_blocks; i++)   
                    printf("Memory block #%u: 0x%04X-0x%04X\n", i+1, ident.mem_start[i], ident.mem_end[i]-1);   
                printf("Erase block size: %3d bytes\n", ident.erblk);   
                printf("Write block size: %3d bytes\n", ident.wrblk);   
                printf("Original vector table: 0x%04X\n",ident.int_vect_tbl);   
                printf("Bootloader user table: 0x%04X\n",ident.bl_tbl);   
            }   
            else if ((bl_version == BL_HCS08_LARGE) || (bl_version == BL_HCS08_LONG))// large + long S08   
            {   
                unsigned i;   
                printf("System device ID: 0x%03X [%s] rev. %u\n", ident.sdid, chipid ,ident.sdidrev);   
                printf("Number of memory blocks: %u\n", ident.num_blocks);   
                for(i=0; i<ident.num_blocks; i++)   
                    printf("Memory block #%u: 0x%06X-0x%06X\n", i+1, ident.mem_start[i], ident.mem_end[i]-1);   
                printf("Erase block size: %3d bytes\n", ident.erblk);   
                printf("Write block size: %3d bytes\n", ident.wrblk);   
                printf("Original vector table: 0x%04X\n",ident.int_vect_tbl);   
                printf("Bootloader user table: 0x%04X\n",ident.bl_tbl);   
            }   
            else   
            {   
                fprintf(stderr, "This version [0x%02X] of protocol is not (yet) supported! Check AN2295SW for update!\n", bl_version);   
                exit(9);   
            }   
        }   
        else    
        {   
            fprintf(stderr, "Can't read MCU info. Could be protocol error.\n");   
            fprintf(stderr, "(Or forgot to set single wire mode?)\n");   
            exit(5);   
        }   
           
        // masquerade vector table   
        if(setup_vect_tbl() < 0)   
            exit(6);   
       
        // check if code goes to valid memory only - issue warning only, but continue! (r30323)   
        check_image();   
       
        if (!defaultyes)   
        {      
            // last bless   
            printf("\nAre you sure to program part? [y/N]: ");   
            if(toupper(getchar()) != 'Y') exit(-1);   
        }   
        else   
        {   
            putchar('\n');   
        }   
       
        // program it!   
        if(prg_mem() < 0)   
        {   
            fprintf(stderr, "Program memory failed.\n");   
            exit(8);   
        }   
       
        // exit program mode, start application   
        unhook();   
       
        close_uart();   
        return 0;   
    }  


