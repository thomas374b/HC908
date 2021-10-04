/*
 * menu_dialogs.c
 *
 *  Created on: 25.07.2020
 *      Author: pantec
 */

#include "menu_dialogs.h"

#include "display.h"

static const dialog_states_e menu_back[menu_total_items] = {
		/*menu_idle             	*/menu_idle,
		/*menu_selection           	*/menu_idle,
		/*menu_plus	         	 	*/menu_idle,
		/*menu_minus	            */menu_idle,

		/*menu_show_clock          	*/menu_selection,
		/*menu_show_adc_selection  	*/menu_selection,
		/*menu_set_relay_selection 	*/menu_selection,
		/*menu_set_pwm			 	*/menu_selection,

		/*menu_show_adc0           	*/menu_show_adc_selection,
		/*menu_show_adc1           	*/menu_show_adc_selection,
		/*menu_show_adc2           	*/menu_show_adc_selection,
		/*menu_show_adc3           	*/menu_show_adc_selection,

		/*menu_set_relay0			*/menu_set_relay_selection,
		/*menu_set_relay1          	*/menu_set_relay_selection,
		/*menu_set_relay2          	*/menu_set_relay_selection,
		/*menu_set_relay3          	*/menu_set_relay_selection,
		/*menu_set_relay4          	*/menu_set_relay_selection,
		/*menu_set_relay5          	*/menu_set_relay_selection,

		// not yet implemented
		/*menu_set_clock_sec		*/menu_idle,
		/*menu_set_clock_min       	*/menu_idle,
		/*menu_set_clock_hour      	*/menu_idle,

		/*menu_set_date_day        	*/menu_idle,
		/*menu_set_date_month      	*/menu_idle,
		/*menu_set_date_year       	*/menu_idle,

		/*menu_set_date 	        */menu_idle,
		/*menu_set_clock  	     	*/menu_idle
};



static const uint8_t menuTitle[] = "menu\0";

static const uint8_t underScore[]  = "_\0";
static const uint8_t actionTitle[] = "action\0";
static const uint8_t relayTitle[]  = "relay\0";
static const uint8_t adcTitle[]    = "adc\0";
static const uint8_t clockTitle[]  = "clock\0";
static const uint8_t pwmTitle[]    = "pwm\0";

static const uint8_t showClockTxt[]   = "show clock\0";
static const uint8_t setClockTxt[]    = "set clock\0";
static const uint8_t showAdcTxt[]     = "show ADC\0";
static const uint8_t setRelayTxt[]    = "set relay\0";
static const uint8_t setPwmTxt[]      = "set pwm\0";

#if 0
static const uint16_t display_text_ptr[menu_total_items] = {
		/*menu_idle             	*/0,
		/*menu_selection           	*/0,
		/*menu_plus	         	 	*/0,
		/*menu_minus	            */0,

		/*menu_show_clock          	*/0,
		/*menu_show_adc_selection  	*/0,
		/*menu_set_relay_selection 	*/0,
		/*menu_set_pwm			 	*/0,

		/*menu_show_adc0           	*/0,
		/*menu_show_adc1           	*/0,
		/*menu_show_adc2           	*/0,
		/*menu_show_adc3           	*/0,

		/*menu_set_relay0			*/0,
		/*menu_set_relay1          	*/0,
		/*menu_set_relay2          	*/0,
		/*menu_set_relay3          	*/0,
		/*menu_set_relay4          	*/0,
		/*menu_set_relay5          	*/0,

		// not yet implemented
		/*menu_set_clock_sec		*/0,
		/*menu_set_clock_min       	*/0,
		/*menu_set_clock_hour      	*/0,

		/*menu_set_date_day        	*/0,
		/*menu_set_date_month      	*/0,
		/*menu_set_date_year       	*/0,

		/*menu_set_date 	        */0,
		/*menu_set_clock  	     	*/0
};
#endif


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
	return 0;
}


void writeTopic(dialog_states_e tpc)
{
	uint8_t *topic = topicAddr(tpc);
	if (topic != 0) {
		display_writeString(topic);
	} else {
		switch(tpc) {
			case menu_show_adc0:
			case menu_show_adc1:
			case menu_show_adc2:
			case menu_show_adc3:
				display_writeString(showAdcTxt);
				display_write(tpc + ('0'-menu_show_adc0));
				break;

			case menu_set_relay0:
			case menu_set_relay1:
			case menu_set_relay2:
			case menu_set_relay3:
			case menu_set_relay4:
			case menu_set_relay5:
			case menu_set_relay6:
			case menu_set_relay7:
				display_writeString(setRelayTxt);
				display_write(tpc + ('0'-menu_set_relay0));
				break;
		}
	}
}


void writeTitle()
{
	display_setCursor(0,0);
	display_writeString(topicTitle(menu.currentIdx));
	display_write(' ');
	display_writeString(&menuTitle[0]);
//	display_writeString(_triSpaces);
}


void showMenu()
{
	writeTitle();

	uint8_t ti;

	ti = menu.choosen+1;
	if (ti > menu.to) {
		ti = menu.from;
	}
	display_setCursor(0,1);
	display_write(' ');
	writeTopic(ti);		// write next menu topic
	display_writeString(_triSpaces);

	display_setCursor(0,2);
	display_write('>');
	writeTopic(menu.choosen);
	display_write('<');
	display_writeString(_triSpaces);

	ti = menu.choosen-1;
	if (ti < menu.from) {
		ti = menu.to;
	}
	display_setCursor(0,3);
	display_write(' ');
	writeTopic(ti);		// write prev menu topic
	display_writeString(_triSpaces);

}


// inline
uint8_t cancelOkAction(uint8_t last)
{
	if (last == keyEsc) {
		if (menu.currentIdx < menu_total_items) {
			menu.currentIdx = menu_back[menu.currentIdx];
		} else {
			menu.currentIdx = menu_idle;
		}
		if (menu.currentIdx == menu_idle) {
			display_setSymbol(symMenu, symOFF);
			display_clear();
			return true;
		}
	} else {
		if (last == keyOK) {
			if (menu.choosen < menu_total_items) {
				menu.currentIdx = menu.choosen;

				switch(menu.currentIdx) {
					case menu_show_clock:
						display_clear();
						writeTitle();
						return true;

					case menu_selection:
					case menu_show_adc_selection:
						showMenu();
						return true;

					case menu_show_adc0:
					case menu_show_adc1:
					case menu_show_adc2:
					case menu_show_adc3:
						// show adc
						break;

					case menu_set_relay_selection:
					case menu_set_relay0:
					case menu_set_relay1:
					case menu_set_relay2:
					case menu_set_relay3:
					case menu_set_relay4:
					case menu_set_relay5:
					case menu_set_relay6:
					case menu_set_relay7:
						// show relay
						break;
				}
			}
		} else {
			// some other key was pressed
		}
	}
	return false;
}




// inline
tristate_e upDownAction(uint8_t last, dialog_states_e from, dialog_states_e to)
{
	menu.from = from;
	menu.to = to;

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
			break;
	}
	if (menu.choosen > menu.to) {
		menu.choosen = menu.from;
	}
	if (menu.choosen < menu.from) {
		menu.choosen = menu.to;
	}
	showMenu();
	return result;
}


// inline
uint8_t leftRightAction(uint8_t last, dialog_states_e from, dialog_states_e to)
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
	showMenu();
	return result;
}




void setRelay(uint8_t idx, tristate_e dir)
{
	uint8_t msk;
	idx -= menu_set_relay0;

	if (idx <= 5) {
		msk = (1 << idx);
		if (dir == triPlus) {
			PWMOUT |= msk;
		} else {
			PWMOUT &= ~msk;
		}
	} else {
		idx -= 6;
		msk = (1 << idx);
		if (dir == triPlus) {
			PORTE |= msk;
		} else {
			PORTE &= ~msk;
		}
	}
}


void adjustClock(uint8_t idx, tristate_e dir)
{
	idx = 0;
	dir = 0;
}


void onOffSelection(uint8_t last)
{
	if (!cancelOkAction(last)) {
		tristate_e upDn;
		switch(menu.currentIdx) {
			case menu_idle:
				menu.currentIdx = menu_selection;
				display_setSymbol(symMenu, symON);
				menu.choosen = menu_show_clock;
			case menu_selection:
				upDownAction(last, menu_show_clock, menu_set_relay_selection);
				break;

			case menu_show_adc_selection:
				upDownAction(last, menu_show_adc0, menu_show_adc3);
				break;

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
				break;

			case menu_set_clock_sec:
			case menu_set_clock_min:
			case menu_set_clock_hour:
				upDn = upDownAction(last, menu_minus, menu_plus);
				if (upDn != triZero) {
					adjustClock(menu.currentIdx, upDn);
				}
			case menu_set_clock:
				leftRightAction(last, menu_set_clock_sec, menu_set_clock_hour);
				break;

			case menu_set_date_day:
			case menu_set_date_month:
			case menu_set_date_year:
				upDn = upDownAction(last, menu_minus, menu_plus);
				if (upDn != triZero) {
					adjustClock(menu.currentIdx, upDn);
				}
			case menu_set_date:
				leftRightAction(last, menu_set_date_day, menu_set_date_year);
				break;

			case menu_show_clock:
			case menu_set_pwm:
				break;
		}
//		showMenu();
	}
}






