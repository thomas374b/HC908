/*
 * globalVars.h
 *
 *  Created on: 22.07.2020
 *      Author: pantec
 */

#ifndef GLOBALVARS_H_
#define GLOBALVARS_H_

#endif /* GLOBALVARS_H_ */


volatile	timer_data_t 			timer_data;
			timer_uint_t			timer0;
			timer_uint_t			timer1;
#ifdef WITH_DISPLAY
volatile 	spi_receiver_data_t 	spi_receiver;
			lcd_data_t 				mcc315_data;
			keyMatrix_data_t 		keyData;
#endif
#ifdef WITH_UART
volatile 	simple_uart_receiver_t	uart_receiver;
#endif
			i2c_data_t				i2c_data;

			uint8_t					clockGood;
			uint8_t 				loopCount;

			uint8_t					inputFlags;
			uint8_t					spiData;
			uint8_t					spiFlags;

			uint8_t					menuIdx;
			uint8_t					menuChoose;

#ifdef WITH_ADC_CHANNELS
volatile 	average_parms_t 		adc_data;
#endif

#ifdef WITH_STACK_TEST
			uint16_t				stackLow;
#endif
//			sliding_avg_parms_t		timeAvg;
//			uint8_t					strng[32];
//			timer_uint_t			timeout2;
//volatile	uint8_t					relay8_mask;

			 dialog_data_t			menu;



#define		GLOBAL_VARS_START		(uint16_t)&timer_data


