/*
 * test-02.c
 *
 *  Created on: 12.06.2020
 *      Author: pantec
 */

#include <hc08/mc68hc908jkjl.h>

#include "timer_hc908.h"
#include "adc_hc908.h"

#include "dumb_uart.c"
#include "byte8_average.c"


unsigned char _sdcc_external_startup()
{
	CONFIG1 |= 1;		// disable COP

	IMASKK = 1;			// disable keyb interrupts
	IMASK1 = 1;			// disable external interrupts
	BRKSCR = 0;			// disable break interrupts
	KBIER = 0;			// disable keyb interrupt pins
//	ADSCR = 0;			// disable ADC interrupts
	CH0IE = 0;			// disable timer0 interrupts
	CH1IE = 0;			// disable timer1 interrupts

	timer_init();
	adc_init();

	return 0;
}



#define		STABLE_GRADIENT		2
#define		ADC_RANGE			4
/* gerechnet aus Projekt.gnumeric
#define		ADC_KEY_cup			 94
#define		ADC_KEY_P			109
#define		ADC_KEY_HEAT		123
#define		ADC_KEY_CUP			149
#define		ADC_KEY_POWER		176
#define		ADC_KEY_C			235
*/

// gemessen
#define		ADC_KEY_cup			110
#define		ADC_KEY_P			 95
#define		ADC_KEY_HEAT		151
#define		ADC_KEY_CUP			125
#define		ADC_KEY_POWER		240
#define		ADC_KEY_C			180

#define		KEY_IN_RANGE(x,y)	(((y-ADC_RANGE) < x) && (x < (y+ADC_RANGE)))
#define		RELAY_OP(b, p)		if ((relay_status & _BV(b)) > 0) {relay_status &= ~_BV(b); p = 0; } else { relay_status |= _BV(b); p = 1; }


typedef struct {
	uint8_t detect_enter;
//	uint16_t detect_enter;
} adc_key_data_t;


volatile average8_parms_t 	adc_data;
volatile timer_data_t 		timer_data;
timer_int_t_ timeout0;
timer_int_t_ timeout1;
adc_key_data_t adc_keys;
uint8_t relay_status;

// #error "exit here"

#define		GLOBALS_SIZE  ( sizeof(average8_parms_t) +\
							sizeof(timer_data_t) +\
							2*sizeof(timer_int_t_) +\
							sizeof(adc_key_data_t) +\
							sizeof(uint8_t)		)

// global.flags
	// adc_filled
	// key_detect_enter

// relays

#if 0
#define		PRINT_BUF_SIZE		6

/**
 * needs (310 + (N*10)) bytes progmem
 */
void uart_puti(int16_t l
#ifdef WITH_DOT_PRINT
							, uint8_t dot
#endif
		)
{
//	char _buf[PRINT_BUF_SIZE];
//	debug_toggle();
#ifdef USE_STD_SPRINTF
	sprintf(buf,"\t% 5ld ", l);
	uart_puts(buf);
#else
	uint8_t j,k;

//	memset(_buf, 0, PRINT_BUF_SIZE);
//	_buf[0] = '\0';

	k = 0;
	j = 1;
	if (l < 0) {
		l = -l;
		k = 1;
	}
	while ((l > 0) && (j < _print_buf_size2)) {
		uint8_t d = l % 10;
		l = l / 10;
#ifdef WITH_DOT_PRINT
		if (dot > 0) {
			if (j == dot) {
				buf[j++] = '.';
			}
		}
#endif
		dumb_uart_putc(d+48);
//		_buf[j++] = d + 48;
	}
	if (j == 1) {
//		_buf[j++] = 48;
		dumb_uart_putc(48);
	}
	if (k > 0) {
//		_buf[j++] = '-';
		dumb_uart_putc('-');
	}
	_buf[j++] = '\t';

	char *s  = &_buf[j-1];
    while (*s)
    {   /* so lange *s != '\0' also ungleich dem "String-Endezeichen(Terminator)" */
    	uart_putc((char)*s);
        s--;
    }
#endif // USE_STD_SPRINTF
}
#endif



#define PRINTD_16 1

#ifdef PRINTD_16
int16_t uart_print_data;
void uart_printd_func()
{
	uint8_t z  = ' ';

	if (uart_print_data < 0) {
		z = '-';
		uart_print_data = -uart_print_data;
	}

	int16_t d = 10000;

	do {
		int16_t m = uart_print_data / d;

		if (m > 0) {
			if (z != '0') {
				dumb_uart_putc(z);
			}
			dumb_uart_putc((m % 0xFF)+48);
			z = '0';
		} else {
			if (z == '-') {
				dumb_uart_putc(' ');
			} else {
				if (d == 1) {
					dumb_uart_putc('0');
				} else {
					dumb_uart_putc(z);
				}
			}
		}
		m *= d;
		uart_print_data -= m;

		d /= 10;
	} while (d > 0);

	dumb_uart_putc('\t');

}

void uart_printx()
{
}

void uart_printb_func()
{
	uint8_t z  = ' ';
	uint8_t i;

//	dumb_uart_putc('0');
//	dumb_uart_putc('b');

	for (i=0x80; i>0; i >>= 1) {
		if ((uart_print_data & i) > 0) {
			dumb_uart_putc('1');
		} else {
			dumb_uart_putc('_');
		}
	}
	dumb_uart_putc('\t');
}



#else

int8_t uart_print_data;
void uart_printd_func()
{
	if (uart_print_data < 0) {
		dumb_uart_putc('-');
		uart_print_data = -uart_print_data;
	}

	int8_t d = 100;

	do {
		int8_t m = uart_print_data / d;

		if (m > 0) {
			dumb_uart_putc((m % 0xFF)+48);
		} else {
			dumb_uart_putc(' ');
		}
		m *= d;
		uart_print_data -= m;

		d /= 10;
	} while (d > 0);
	dumb_uart_putc('\t');
}
#endif

#define	uart_printd(x)	{ uart_print_data = x; uart_printd_func(); }
#define	uart_printb(x)	{ uart_print_data = x; uart_printb_func(); }

int main()
{
	DDRB = 0xFF & ~((1<<0)|(1<<2)); 				// 1,3,4,5,6,7 on
	DDRD = 0xFF & ((1<<2) | (1<<5) | (1<<6) | (1<<7));

	dumb_uart_putc(RSR);

	uint8_t i;
	uint8_t *b = (uint8_t *)&adc_data;
	for (i=0; i<GLOBALS_SIZE; i++) {
		*b = 0;
		b++;
	}

	__asm
		cli
	__endasm;


	do {
//		COP_reset();
		uint8_t avg;

		getTime();		// 80 µs

		if (AIEN == 0) {
			LED_on();
			byte32Average(&adc_data);

			avg = adc_data.av;

			if ((adc_data.gradient < STABLE_GRADIENT)
//				&& (adc_data.gradient > -STABLE_GRADIENT)) {  // key press and release
				&& (adc_data.gradient >= 0)) {		// only keypress
				// stabilized

				if (!adc_keys.detect_enter) {
					adc_keys.detect_enter = 1;

					uint8_t old_relays = relay_status;

					if KEY_IN_RANGE(avg, ADC_KEY_POWER) {
						RELAY_OP(1, RELAY1);
					} else {
						if KEY_IN_RANGE(avg, ADC_KEY_C) {
							RELAY_OP(2, RELAY2);
						} else {
							if KEY_IN_RANGE(avg, ADC_KEY_P) {
								RELAY_OP(3, RELAY3);
							} else {
								if KEY_IN_RANGE(avg, ADC_KEY_HEAT) {
									RELAY_OP(4, RELAY4);
								} else {
									if KEY_IN_RANGE(avg, ADC_KEY_cup) {
										RELAY_OP(5, RELAY5);
									} else {
										if KEY_IN_RANGE(avg, ADC_KEY_CUP) {
											RELAY_OP(6, RELAY6);
										} else {
											// key released
										}
									}
								}
							}
						}
					}
					if (old_relays != relay_status) {
						dumb_uart_putc('E');
						uart_printd(avg);
						uart_printb(relay_status);
						dumb_uart_putc('\n');
						old_relays = relay_status;
					}
				}
			} else {
				if (adc_data.gradient < 0) {
					adc_data.gradient = -adc_data.gradient;
				}
				if (adc_data.gradient > (STABLE_GRADIENT*3)) {
					// unstable again
					if (adc_keys.detect_enter) {
						adc_keys.detect_enter = 0;
					}
				} else {
				}
			}


			LED_off();
/*
			dumb_uart_putc('A');
			uart_printd(adc_data.raw);
			uart_printd(avg);
			uart_printd(adc_data.gradient);
			dumb_uart_putc('\n');
*/
			AIEN = 1;
//			continue;
		}

//		LED_on();
//		LED_off();

		if (at_interval(1000, &timeout0)) {
//			AIEN = 0;


			dumb_uart_putc('T');
//			dumb_uart_putc(timer_data.global_timer_wraps);
//			dumb_uart_putc('A');
//			dumb_uart_putc(adc_data.raw);

			uart_printd(timer_data.now);

			uart_printd(adc_data.raw);
			uart_printd(avg);
			uart_printd(adc_data.gradient);

			dumb_uart_putc('\n');

//			AIEN = 1;
		}
/*
 * 		// generic timer test
		if (at_interval(250, &timeout1)) {
			if (PTD5 > 0) {
				PTD5 = 0;
			} else {
				PTD5 = 1;
			}
		}
*/
	} while (1);
}





#include "nakedISR_hc908.h"

#include "compiler_lib.h"



