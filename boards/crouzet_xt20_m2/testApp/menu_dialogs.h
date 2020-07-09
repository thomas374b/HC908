/*
 * menu_dialogs.h
 *
 *  Created on: 30.06.2020
 *      Author: pantec
 */

#ifndef MENU_DIALOGS_H_
#define MENU_DIALOGS_H_


#include "display.h"


typedef enum {
	menu_idle                 =    0,
	menu_selection            = 0x80,

	menu_show_clock           = 0x41,
	menu_show_adc_selection   = 0x42,
	menu_set_relay_selection  = 0x43,
	menu_set_pwm			  = 0x44,

	menu_show_adc0            = 0x20,
	menu_show_adc1            = 0x21,
	menu_show_adc8            = 0x22,
	menu_show_adc9            = 0x23,

	menu_set_relay0			  = 0x10,
	menu_set_relay1           = 0x11,
	menu_set_relay2           = 0x12,
	menu_set_relay3           = 0x13,
	menu_set_relay4           = 0x14,
	menu_set_relay5           = 0x15,
	menu_set_relay6           = 0x16,
	menu_set_relay7           = 0x17,

	menu_plus	         	  = 0x01,
	menu_minus	              = 0x02,

	menu_set_clock_sec		  = 0x30,
	menu_set_clock_min        = 0x31,
	menu_set_clock_hour       = 0x32,
	menu_set_date_day         = 0x33,
	menu_set_date_month       = 0x34,
	menu_set_date_year        = 0x35,

	menu_set_date 	          = 0x36,
	menu_set_clock  	      = 0x37,
} dialog_states_e;


typedef enum {
	triZero = 0,
	triPlus = 1,
	triMinus = 2,
} tristate_e;

typedef struct {
	dialog_states_e currentIdx;
	uint8_t choosen;
} dialog_data_t;


extern dialog_data_t	menu;



// inline
tristate_e upDownAction(uint8_t last, uint8_t from, uint8_t to)
{
	tristate_e result = triZero;
	switch(last) {
		case keyPlus:	// up
			menu.choosen++;
			result = triPlus;
			break;

		case keyMinus:	// down
			menu.choosen--;
			result = triMinus;
			break;

		default:
			return result;
	}
	if (menu.choosen > to) {
		menu.choosen = from;
	}
	if (menu.choosen < from) {
		menu.choosen = to;
	}
	return result;
}

// inline
uint8_t leftRightAction(uint8_t last, uint8_t from, uint8_t to)
{
	uint8_t result = true;
	switch(last) {
		case keyB:		// right
			menu.choosen++;
			break;

		case keyA:		// left
			menu.choosen--;
			break;

		default:
			result = false;
			break;
	}
	if (menu.choosen > to) {
		menu.choosen = from;
	}
	if (menu.choosen < from) {
		menu.choosen = to;
	}
	return result;
}


inline
void cancelOkAction(uint8_t last)
{
	if (last == keyEsc) {
		switch(menu.currentIdx) {
			case menu_set_clock_sec:
			case menu_set_clock_min:
			case menu_set_clock_hour:
				menu.currentIdx = menu_set_clock;
				break;

			case menu_set_date_day:
			case menu_set_date_month:
			case menu_set_date_year:
				menu.currentIdx = menu_set_date;
				break;

			case menu_set_clock:
			case menu_set_date:
				menu.currentIdx = menu_show_clock;
				break;

			case menu_show_clock:
				menu.currentIdx = menu_selection;
				break;

			case menu_set_relay0:
			case menu_set_relay1:
			case menu_set_relay2:
			case menu_set_relay3:
			case menu_set_relay4:
			case menu_set_relay5:
			case menu_set_relay6:
			case menu_set_relay7:
				menu.currentIdx = menu_set_relay_selection;
				break;

			case menu_set_relay_selection:
			case menu_selection:
				menu.currentIdx = menu_idle;
				display_setSymbol(symMenu, symOFF);
				break;
		}
	} else {
		switch(menu.currentIdx) {
			case menu_selection:
				menu.currentIdx = menu.choosen;
				break;
		}
	}
}

static const uint8_t menuTitle[] = "menu\0";

static const uint8_t underScore[]  = "_\0";
static const uint8_t actionTitle[] = "action\0";
static const uint8_t relayTitle[]  = "relay\0";
static const uint8_t adcTitle[]    = "adc\0";
static const uint8_t clockTitle[]  = "clock\0";

static const uint8_t showClockTxt[]   = "show clock\0";
static const uint8_t setClockTxt[]    = "set clock\0";
static const uint8_t showAdcTxt[]     = "show ADC\0";
static const uint8_t setRelayTxt[]    = "set relay\0";


uint8_t *topicTitle(uint8_t idx)
{
	switch(idx) {
		case menu_selection:		   return &actionTitle[0];

		case menu_set_clock:		   return &clockTitle[0];
		case menu_show_clock:		   return &clockTitle[0];
		case menu_show_adc_selection:  return &adcTitle[0];
		case menu_set_relay_selection: return &relayTitle[0];
	}
	return &underScore[0];
}

uint8_t *topicAddr(uint8_t idx)
{
	switch(idx) {
		case menu_show_clock:		   return &showClockTxt[0];
		case menu_set_clock:		   return &setClockTxt[0];
		case menu_show_adc_selection:  return &showAdcTxt[0];
		case menu_set_relay_selection: return &setRelayTxt[0];
	}
	return &underScore[0];
}




void showMenu()
{
	display_setSymbol(symMenu, symON);

	display_setCursor(0,0);
	display_writeString(topicTitle(menu.currentIdx));
	display_write(' ');
	display_writeString(&menuTitle[0]);

	display_setCursor(0,1);
	display_write(' ');
	// TODO: write prev menu topic

	display_setCursor(0,2);

	display_write('>');
	display_writeString(topicAddr(menu.currentIdx));

	display_write('<');
	display_setCursor(0,3);

	display_write(' ');
	// TODO: write next menu topic
}



void setRelay(uint8_t idx, tristate_e dir)
{
}

void adjustClock(uint8_t idx, tristate_e dir)
{
}


void onOffSelection(uint8_t last)
{
	tristate_e upDn;

	switch(menu.currentIdx) {
		case menu_selection:
			upDownAction(last, menu_show_clock, menu_set_relay_selection);
			return;

		case menu_show_adc_selection:
			upDownAction(last, menu_show_adc0, menu_show_adc9);
			return;

		case menu_set_relay0:
		case menu_set_relay1:
		case menu_set_relay2:
		case menu_set_relay3:
		case menu_set_relay4:
		case menu_set_relay5:
		case menu_set_relay6:
		case menu_set_relay7:
			upDn = upDownAction(last, menu_minus, menu_plus);
			if (upDn != triZero) {
				setRelay(menu.currentIdx, upDn);
			}
		case menu_set_relay_selection:
			leftRightAction(last, menu_set_relay0, menu_set_relay7);
			return;

		case menu_set_clock_sec:
		case menu_set_clock_min:
		case menu_set_clock_hour:
			upDn = upDownAction(last, menu_minus, menu_plus);
			if (upDn != triZero) {
				adjustClock(menu.currentIdx, upDn);
			}
		case menu_set_clock:
			leftRightAction(last, menu_set_clock_sec, menu_set_clock_hour);
			return;

		case menu_set_date_day:
		case menu_set_date_month:
		case menu_set_date_year:
			upDn = upDownAction(last, menu_minus, menu_plus);
			if (upDn != triZero) {
				adjustClock(menu.currentIdx, upDn);
			}
		case menu_set_date:
			leftRightAction(last, menu_set_date_day, menu_set_date_year);
			return;

		case menu_set_pwm:
			break;
	}
}





#endif /* MENU_DIALOGS_H_ */
