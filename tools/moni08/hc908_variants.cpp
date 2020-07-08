/*
 * hc908_variants.cpp
 *
 *  Created on: 11.06.2020
 *      Author: pantec
 */


#include "hc908_variants.h"


const cpu_firmware_addr_t cpu_firmware_addr[N_HC908_VARIANTS] = {
		{
				 /*.idx =		*/	mc68hc908jk1,
				 /*.CtrlByte =	*/	0x88,		// source: Freescale doc
				 /*.cpuSpeed = 	*/	0x89,		// source: Freescale doc
				 /*.Laddr = 	*/	0x8A,		// source: Freescale doc
				 /*.dataBuf = 	*/	0x8C,		// source: Freescale doc
				 /*.dataSize = 	*/	64,			// source: Freescale doc
				 /*.flashStart =*/	0xF600,		// source: Freescale doc
				 /*.flashSize = */	1536,		// source: Freescale doc
				 /*.vectorStart=*/  0xFFD0,		// source: Freescale doc
				 /*.FLBPR =		*/	0xFE09,		// source: Freescale doc
				 /*.erase = 	*/	0xFC06,		// source: Freescale doc
				 /*.program = 	*/	0xFC09,		// source: Freescale doc
				 /*.HX =		*/	0x86,		// source: RAM bootlader code
				 /*.jumpAddr =	*/	0xCF,		// source: RAM bootlader code
				 /*.ramStart =	*/	0x080,		// source: Freescale doc
				 /*.ramSize =	*/	128,		// source: Freescale doc
				 /*.secBytes =  */  {8, {0xFFF6, 0xFFF7, 0xFFF8, 0xFFF9, 0xFFFA, 0xFFFB, 0xFFFC, 0xFFFD}},
				 	 	 	 	 	 	 // IRQ vector addresses TIM0, n.u., IRQ, SWI
				 /*.variantStr =*/	"hc908jk1"
		 }
		,{
				 /*.idx = 		*/	mc68hc908jk3,
				 /*.CtrlByte =	*/	0x88,		// source: Freescale doc
				 /*.cpuSpeed =	*/	0x89,		// source: Freescale doc
				 /*.Laddr =		*/	0x8A,		// source: Freescale doc
				 /*.dataBuf =	*/	0x8C,		// source: Freescale doc
				 /*.dataSize =	*/	64,			// source: Freescale doc
				 /*.flashStart =*/	0xEC00,		// source: Freescale doc
				 /*.flashSize =	*/	4096,		// source: Freescale doc
				 /*.vectorStart=*/  0xFFD0,		// source: Freescale doc
				 /*.FLBPR = 	*/	0xFE09,		// source: Freescale doc
				 /*.erase = 	*/	0xFD2B,		// source: CPU monitor ROM
				 /*.program = 	*/	0xFC77,		// source: CPU monitor ROM
				 /*.HX = 		*/	0x86,		// source: RAM bootlader code
				 /*.jumpAddr = 	*/	0xCF,		// source: RAM bootlader code
				 /*.ramStart =	*/	0x080,		// source: Freescale doc
				 /*.ramSize =	*/	128,		// source: Freescale doc
				 /*.secBytes =  */  {8, {0xFFF6, 0xFFF7, 0xFFF8, 0xFFF9, 0xFFFA, 0xFFFB, 0xFFFC, 0xFFFD}},
				 	 	 	 	 	 	 // IRQ vector addresses TIM0, n.u., IRQ, SWI
				 /*.variantStr =*/	"hc908jk3"
		}
		,{
				 /*.idx = 		*/	mc68hc908jb8,
				 /*.CtrlByte = 	*/	0x48,		// source: Freescale doc
				 /*.cpuSpeed = 	*/	0x49,		// source: Freescale doc
				 /*.Laddr = 	*/	0x4A,		// source: Freescale doc
				 /*.dataBuf = 	*/	0x4C,		// source: Freescale doc
				 /*.dataSize = 	*/	64,			// row programming, 512 bytes minimum erase size
				 /*.flashStart =*/	0xDC00,		// source: Freescale doc
				 /*.flashSize = */	8192,		// source: Freescale doc
				 /*.vectorStart=*/  0xFFF0,		// source: Freescale doc
				 /*.FLBPR = 	*/	0xFE09,		// source: Freescale doc
				 /*.erase = 	*/	0xFC06,		// source: Freescale doc
				 /*.program = 	*/	0xFC09,		// source: Freescale doc
				 /*.HX = 		*/	0x46,		// source: RAM bootlader code
				 /*.jumpAddr = 	*/	0x8F, 		// source: RAM bootlader code
				 /*.ramStart =	*/	0x040,		// source: Freescale doc
				 /*.ramSize =	*/	256,		// source: Freescale doc
				 	 	 	 	 	 	 	// source: Freescale doc
				 /*.secBytes =  */  {8, {0xFFF6, 0xFFF7, 0xFFF8, 0xFFF9, 0xFFFA, 0xFFFB, 0xFFFC, 0xFFFD}},
				 	 	 	 	 	 	 // IRQ vector addresses TIM0, n.u., IRQ, SWI
				 /*.variantStr =*/	"hc908jb8"	//
		}

		,{
				 /*.idx = 		*/	mc68hc908mr32,
				 /*.CtrlByte = 	*/	0x68,		//	source: RAM bootlader code
				 /*.cpuSpeed = 	*/	0x69,		//	source: RAM bootlader code
				 /*.Laddr = 	*/	0x6A,		//  source: RAM bootlader code
				 /*.dataBuf = 	*/	0x6C,		//	source: RAM bootlader code
				 /*.dataSize = 	*/	64,
				 /*.flashStart =*/	0x8000,		//
				 /*.flashSize = */	32256,
				 /*.vectorStart=*/  0xFFD2,		//
				 /*.FLBPR = 	*/	0xFF7E,		//
				 /*.erase = 	*/	0x0100,		// 	source: RAM bootlader code
				 /*.program = 	*/	0x013D,		//	source: RAM bootlader code
				 /*.HX = 		*/	0x66,		//	source: RAM bootlader code
				 /*.jumpAddr = 	*/	0xB5,  		//  source: RAM bootlader code
				 /*.ramStart =	*/	0x60,		//
				 /*.ramSize =	*/	768,		//
				 	 	 	 	//
				 /*.secBytes =  */  {8, {0xFFF6, 0xFFF7, 0xFFF8, 0xFFF9, 0xFFFA, 0xFFFB, 0xFFFC, 0xFFFD}},
				 	 	 	 	 	 	 // IRQ vector addresses TIM0, n.u., IRQ, SWI
				 /*.variantStr =*/	"hc908mr32"	// TODO: assemble new bootloader
		}
};

