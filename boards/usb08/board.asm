


		#include	"platform.asm"
		

		#if	(START < [RAM_START+RAM_SIZE])
; probably running rom RAM in monitor mode
F_CPU	.EQU	3000000

		#else

; running from FLASH in user mode
F_CPU	.EQU	3000000
		#endif


LED0	.EQU	PTD0
LED1	.EQU	PTD1
LED2	.EQU	PTD2

; original settings replaced on my sensor board
PHOTO_ADC		.EQU	PTD3
PHOTO_ADC_CHRG	.EQU	PTE0

TEMP_ADC		.EQU	PTD4
TEMP_ADC_CHRG	.EQU	PTE1

POTI_ADC		.EQU	PTD5
POTI_ADC_CHRG	.EQU	PTE2

COMMON_ADC		.EQU	PTD6

; switch at IRQ pin

KEY0	.EQU	PTA4
KEY1	.EQU	PTA5
KEY2	.EQU	PTA6

USB_DP	.EQU	PTE3
USB_DM	.EQU	PTE4


