/*
 * menu_dialogs.h
 *
 *  Created on: 30.06.2020
 *      Author: pantec
 */

#ifndef MENU_DIALOGS_H_
#define MENU_DIALOGS_H_

typedef enum {
	menu_idle             = 0,
	menu_selection           ,
	menu_plus	         	 ,
	menu_minus	             ,

	menu_show_clock          ,
	menu_show_adc_selection  ,
	menu_set_relay_selection ,
	menu_set_pwm			 ,

	menu_show_adc0           ,
	menu_show_adc1           ,
	menu_show_adc2           ,
	menu_show_adc3           ,

	menu_set_relay0			 ,
	menu_set_relay1          ,
	menu_set_relay2          ,
	menu_set_relay3          ,
	menu_set_relay4          ,
	menu_set_relay5          ,
	menu_set_relay6          ,
	menu_set_relay7          ,

	menu_set_clock_sec		 ,
	menu_set_clock_min       ,
	menu_set_clock_hour      ,
	menu_set_date_day        ,
	menu_set_date_month      ,
	menu_set_date_year       ,

	menu_set_date 	         ,
	menu_set_clock  	     ,

	menu_total_items
} dialog_states_e;


typedef struct {
	dialog_states_e currentIdx;
	dialog_states_e choosen;
	dialog_states_e from;
	dialog_states_e to;
} dialog_data_t;

extern dialog_data_t	menu;


typedef enum {
	triZero = 0,
	triPlus = 1,
	triMinus = 2,
} tristate_e;


#endif /* MENU_DIALOGS_H_ */
