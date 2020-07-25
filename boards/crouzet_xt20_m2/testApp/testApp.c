/*
 * main.c
 *
 *  Created on: 19.06.2020
 *      Author: pantec
 */

#include "compiler_lib.h"

#include "board.h"

#include "uart_printd.h"
#include "uart_printb.h"
#include "uart_printh.h"

#ifdef WITH_ADC_CHANNELS
	#include "adc_hc908.h"
	#include "sliding_int_avg.c"
#endif

#include "display.h"

const static uint8_t _triSpaces[] = "   ";


#ifdef WITH_DIALOGS
	#include "menu_dialogs.h"
	#include "menu_dialogs.c"
#endif



unsigned char _sdcc_external_startup()
{
	CONFIG1 |= 1;           // disable COP
	IMASK1 = 1;                     // disable external interrupts
	BRKSCR = 0;                     // disable break interrupts

	board_init();

	return 0;
}

/*
typedef enum {
	menu_item_mask = 0x30,
} menu_masks_e;
*/



#ifdef WITH_STACK_TEST
volatile __data uint8_t __at 0x80 stackTmpH;
volatile __data uint8_t __at 0x81 stackTmpL;
#endif


// const static char WelcomeMessage[] = "Hallo Thomas\n";



#ifdef WITH_STACK_TEST
void testStack()
{
	__asm
		tsx
		pshh
		pula
		sta	*_stackTmpH
		stx *_stackTmpL
	__endasm;

	uint16_t st = (stackTmpH << 8) | stackTmpL;
	if (st < stackLow) {
		stackLow = st;
	}
}
#endif


inline
void setup()
{
#ifdef WITH_STACK_TEST
	stackLow = 0x360;
#endif

#if defined(LEDPIN_MASK)
	LEDPIN_DDR |= LEDPIN_MASK;
#endif
#if defined(SENDLED_MASK)
	SENDLED_DDR |= SENDLED_MASK;
#endif
	DDRC |= 0xFF;

//	DDRA |= 0x01;
//	DDRF |= 0x02;
//	spiFlags = 0xFE;
}

// const uint8_t ports[6] = {PORTA, PORTB, PORTC, PORTD, PORTE, PORTF};


#ifdef WITH_DISPLAY

inline void keybAction()
{
	uart_putc('\n');
	uint8_t last = keyData.pLast[keyData.page];
	if (last & keyPress) {
#ifdef WITH_DIALOGS
/*
		switch(menuIdx) {
			case menu_idle:
				menuIdx = menu_selection;
				showMenu();
				break;

			case menu_selection:
				break;

			case menu_show_clock:
				break;

			case menu_set_clock:
				break;

			case menu_show_adc_selection:
				break;

			case menu_set_relay_selection:
				break;
		}
*/
#endif
		uart_putc('p');
	} else {
		if (last & keyRelease) {
			last &= keyAllBits;
#ifdef WITH_DIALOGS
			onOffSelection(last);
#endif
			uart_putc('r');
		} else {
		}
	}
	uart_printh(last & keyAllBits);
	uart_printh(menuIdx);
	uart_printh(menuChoose);
}

#endif


inline
void trigger200action()
{
/*
				uart_printd(timer_data.now);
				uart_printd(timer0);
				uart_printd(MS_TO_GAP(200UL));
				uart_putc('\n');
*/
				uart_printb(PORTA);		// control
				uart_printb(PORTB);		// inputs I5..I12
				uart_printb(PORTC);		// input bits
//				uart_printb(PORTD);		// maybe keyboard
//				uart_printb(PORTE);		// inputs I2..I4, 2 missing relays ?
//				uart_printb(PORTF);
//				uart_printb(PWMOUT);

				uart_putc('\n');
}


inline
void trigger1000action()
{
#ifdef WITH_M41T56_CLOCKCHIP
	uint8_t i;
	uint8_t r;
#endif

	uart_putc('\n');

#if 0
		if (loopCount == 56) {
			uart_putc('\n');

			if (clockGood == 0) {
				uint8_t r = m41t56_read_wa(56, 8);
				if (r == 0) {
					// read OK
					r = 7 + 56;
					for (i=0; i<8; i++) {
						r += i2c_data_buf[i];
						i2c_data_buf[i] = 0;
					}
					if (r != 0xFF) {
						for (i=1; i<8; i++) {
							i2c_data_buf[7] = 255 -7 -(i*8);
							r = m41t56_write( i*8, 8);
						}
						if (r == 0) {
							i2c_data_buf[0] = 0;
							i2c_data_buf[1] = 0x40;
							i2c_data_buf[2] = 0;
							i2c_data_buf[3] = 0;
							i2c_data_buf[4] = 0x21;
							i2c_data_buf[5] = 0x06;
							i2c_data_buf[6] = 0x20;
							i2c_data_buf[7] = 0;

							if (m41t56_write(0, 8) == 0) {
								uart_putc('S');
								clockGood = 1;
							} else {
								uart_putc('C');
								uart_printh(r);
							}
						} else {
							uart_putc('W');
							uart_printh(r);
						}
					} else {
						clockGood = 1;
					}
				} else {
					uart_putc('R');
					uart_printh(r);
				}
				r = m41t56_read_wa(0, 1);
				if (r == 0) {
					if ((i2c_data_buf[0] & 0x80) > 0) {
						i2c_data_buf[0] &= 0x7F;
						m41t56_write(0, 1);
					}
				}
			}

		}
#endif

#ifdef WITH_DISPLAY
#ifdef WITH_PIE_SECONDS_COUNTER
		if (inputFlags > 0) {
			mcc315lcd_setPropeller(loopCount, spiData); // rotor blade
		} else {
			mcc315lcd_setPie(loopCount, spiData); // binary clock
		}
		loopCount++;
#endif
#endif

		uart_putc('\t');
		uart_printd(timer_data.now);

		uart_printd(timer_data.wrap_count);

#ifdef WITH_M41T56_CLOCKCHIP

#ifdef WITH_DISPLAY
		if (menu.currentIdx == menu_show_clock) {
#endif

		i = m41t56_read_wa(0, FIXED_I2C_DATA_LEN);
		if (i == 0) {
/*
			for (i=0; i<FIXED_I2C_DATA_LEN; i++) {
				uart_printh(i2c_data_buf[i]);
			}
*/
			if ((i2c_data.buffer[0] & 0x80) > 0) {  // if clock is stopped
				i2c_data.buffer[0] &= ~0x80;		 // clear stop-bit
				m41t56_write(0, 1);
			}
			i = 0;
			do {
				r = m41t56_getClockByte(i);

				if (i == 0) {
					display_setCursor(0,1);
				}
				if (i == 11) {
					display_setCursor(1,2);
				}
				if (r != 0) {
					display_write(r);
					uart_putc(r);
				}
				i++;
			} while ((r!= 0) && (i < 20));
			mcc315lcd_writeString(&_triSpaces[0]);
		} else {
			uart_putc('c');
			uart_printd(i);
		}
#ifdef WITH_DISPLAY
		}
#endif
//		uart_printd(keyData.msk);
//		uart_printd(keyData.cnt[0]);
//		uart_printd(keyData.cnt[1]);
#endif

		uart_putc('\t');
//		uart_printb(spi_receiver.statusReg);

		uart_printb(PORTC);		// some control bits
//		uart_printb(PORTD);		// keyboard


		if (inputFlags == 0) {
			uart_putc('*');
		}
		uart_printb(spiFlags);
		uart_printh(spiFlags);

		if (inputFlags != 0) {
			uart_putc('*');
		}

		uart_printb(spiData);
		uart_printh(spiData);

		for (i=0; i<4; i++) {
//			uart_printd(i);
			uart_printd(adc_data.avg[i].sum);
#ifdef WITH_TRACE_MIN_MAX
			uart_printd(adc_data.avg[i].max - adc_data.avg[i].min);
#endif
		}

#ifdef WITH_TRACE_MIN_MAX
		if ((loopCount & 0x03) == 0) {
			for (i=0; i<4; i++) {
				resetMinMax(&adc_data.avg[i]);
			}
		}
#endif


#ifdef WITH_STACK_TEST
		testStack();

		uart_printd(GLOBAL_DATA_SIZE);

		uart_printhx(stackLow);
#endif

//		uint8_t i;

/*
		intAverage(&timeAvg, timer_data.now & 0xFF);
		uart_printd(AvgIntValue(timeAvg));
		uart_printd(timeAvg.sum);
		uart_printd(timeAvg.gradient);
*/

//		timeout2 = timer_data.now + 1000 -1;
//		timer_int_t_ delta = (timer_data.now - timeout2);
//		uart_printd(delta);

//		timeout2 = timer_data.now + 500 -1;
//		uart_printd(timeout2);
//		uart_printd( timeout2 < timer_data.now);

//		uart_printd(MS_TO_GAP(1100));
//		uart_printd(MS_TO_GAP(250));


//		i = m41t56_read_wa(loopCount, 8);
//		uart_printh(i);
#if 0
		mcc315lcd_command(lcd_SETDDRAMADDR | ((loopCount>>1) & 0x7F) );
		for (i=0;i<8;i++) {
			i2c_data_buf[i] = ((loopCount & 1) > 0) ? 0x1F : 0x00;
		}
		mcc315lcd_createChar(inputFlags, i2c_data_buf);
		mcc315lcd_write(loopCount);

		uart_printh(loopCount>>1);
		uart_printh(inputFlags);
		uart_printd(loopCount&1);
//		uart_putc('\n');

		inputFlags++;
		inputFlags &= 0x07;
		if (inputFlags == 0) {
			loopCount++;
		}

//		mcc315lcd_command(0x38);
//		mcc315lcd_command(lcd_SETDDRAMADDR | loopCount);
//		mcc315lcd_write(spiData);
//		loopCount++;
//		loopCount &= 0x7F;

		if (i == 0) {
			uart_printd(loopCount);


			loopCount += 8;
			if (loopCount >= 64) {
				loopCount = 0;
			}
		}
#endif

/*
	uart_putc('\t');
		r = spiFlags | 0xFC;
		uart_printh(r);
		spi_write(r);

		r = (loopCount >> 4);
		uart_printh(r);
		spi_write(r);

//		r = loopCount & 0x0F;
		uart_printh(r);
		spi_write(r);

		spiFlags++
		spiFlags &= 0x07;
		if (spiFlags == 0) {
			loopCount++;
		}
*/
}


#define TOGGLE_BIT(port, msk)	{ if ((port & msk) > 0) {	port &= ~msk; } else {	port |= msk; } }

void toggle_relay(uint8_t i)
{
	if (i <= 5) {
		TOGGLE_BIT(PWMOUT, 1 << i);
	} else {
		TOGGLE_BIT(PORTE, 1 << (i-6));
	}
}


inline
void uartAction()
{
	uint8_t i;

	uint8_t rb = nextUartByte();

//		if (rb == '\\') {
//			rb = nextUartByte();

	switch(rb) {
		default:
			mcc315lcd_write(rb);
		case '\n':
			break;
/*
		case 'Z':
			mcc315_init();
			break;
*/
		case 'A':
		case 'B':
//		case 'C':
//		case 'D':
//		case 'E':
		case 'F':
		case 'G':
			rb -= 'A';
			i = (1 << rb);
			TOGGLE_BIT(PORTC, i);
			break;
/*
		case 'F':
			if ((PORTF & 0x02) > 0) {
				// switch off
				PORTF &= ~0x02;
			} else {
				// switch on
				PORTF |= 0x02;
			}
			break;
*/
		case 'H':
			TOGGLE_BIT(PORTA, 1);
			break;

		case 'e':
		case 'f':
		case 'g':
		case 'h':
		case 'i':
		case 'j':
		case 'k':
		case 'l':
			rb -= 'e';
			toggle_relay(rb);
			break;

		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
			rb -= '0';
			i = (1 << rb);
			if (inputFlags != 0) {
				TOGGLE_BIT(spiData, i);
			} else {
				TOGGLE_BIT(spiFlags, i);
			}
			break;

		case '8':
			if (inputFlags != 0) {
				inputFlags = 0;
			} else {
				inputFlags = lcd_Write;
			}
			break;
/*
		case '9':
			rb = 0xF8 | inputFlags | 0x02;
			mcc315lcd_transport(rb);
			uart_printh(rb);
			mcc315lcd_transport(0xFF);
			uart_printh(0xFF);
			uart_putc('\t');
			break;

		case 'r':
			mcc315_data.transferBuf[0] = spiData;
			rb = mcc315lcd_transport(lcd_ReadRAM);
			uart_printh(spiData);
			uart_printh(mcc315_data.transferBuf[2]);
			uart_printh(mcc315_data.transferBuf[1]);
			uart_printh(mcc315_data.transferBuf[0]);
			uart_printh(bitreversed(mcc315_data.transferBuf[0]));
			uart_putc('\t');
			break;
*/
		case 'w':
			mcc315lcd_command(spiData);
			uart_printh(lcd_Command);
			uart_printh(spiData);
			uart_printh(mcc315_data.transferBuf[0]);
			uart_putc('\t');
			break;
/*
		case '+':
			loopCount++;
			loopCount &= 0x7F;
			break;

		case '-':
			loopCount--;
			loopCount &= 0x7F;
			break;
*/
		case 'S':
			mcc315lcd_setSymbol(spiData, spiFlags);
			uart_printh(spiFlags);
			uart_printh(spiData);
			break;


		case 'T':
			mcc315lcd_command(0x38 | spiFlags);
			mcc315lcd_command(lcd_SETDDRAMADDR);	// set DRAM address to zero
			mcc315lcd_command(lcd_SETCGRAMADDR | spiData);
			mcc315lcd_write( 0x3F );
			mcc315lcd_command(0x38);
			break;

		case 'W':
//			uart_puts(sizeof(WelcomeMessage)-1,(uint16_t)&WelcomeMessage);
			break;
/*
		case 'R':
			rb = 0xF8 | inputFlags | 0x02;
			mcc315lcd_transport(rb);
			uart_printh(rb);
			mcc315lcd_transport(0xFF);
			uart_printh(0xFF);
			uart_putc('\t');
			break;

		case 'W':
			rb = 0xF8 | inputFlags;
			mcc315lcd_transport(rb);
			uart_printh(rb);
			mcc315lcd_transport(spiData);
			uart_printh(spiData);
			uart_putc('\t');
			break;



*/
/*
			if ((PORTF & 1) > 0) {
				PORTC &= ~1;
			} else {
				// switch on
				PORTF |= 1;
			}
			break;
*/
	}
//		} else {
//			spi_write(rb);
//		}

}


// #define		SUM_TO_MVOLT(x)			((((x * V_ANALOG_REF) / 320UL) * R_LADDER_PRESCALER) / 100000UL)

inline
uint16_t sum_to_mVolt(uint16_t sum)
{
	uint32_t result = sum;

	result *= V_ANALOG_REF;
	result /= 320;
	result *= R_LADDER_PRESCALER;
	result /= 100000UL;

	return result & 0xFFFF;
}



inline void loop()
{
#ifdef WITH_KEYBOARD
#endif
	uint8_t rb;
#ifdef WITH_ADC_CHANNELS
	uint8_t i;
#endif
//	uint8_t r;

	if (spi_bytesAvailable) {
		rb = nextSpiByte();
		uart_printh(rb);
	}

	if (uart_bytesAvailable) {
		uartAction();
	}

#ifdef WITH_ADC_CHANNELS
	if (AIEN == 0) {
		// ADC has interrupt disabled, i.e. conversion is ready
		i = adc_data.count >> 5;	// 32 counts make one measure before channel switching

		if ((adc_data.count & 0x1F) > 0) {
			intAverage(&adc_data.avg[i], adc_data.raw);

			if (averageStabilizedEvent(&adc_data.avg[i], &adc_data.stabi[i])) {
				// value @ channel i stabilized

				uart_printd(i);
				uart_printd(sum_to_mVolt(adc_data.avg[i].sum));
				uart_putc('m');
				uart_putc('V');

//				uart_printd(adc_data.count);
//				uart_printd(adc_data.avg[i].sum);
//				uart_printd(AvgIntValue(adc_data.avg[i]));
//				uart_printd(Gradient(adc_data.avg[i]));
//				uart_printd(adc_data.avg[i].accel);
				uart_putc('\n');
			}
#ifdef WITH_TRACE_MIN_MAX
			if (adc_data.stabi[i].isStable) {
				int16_t mavg = adc_data.avg[i].gradient;
				if (adc_data.avg[i].min > mavg) {
					adc_data.avg[i].min = mavg;
				}
				if (adc_data.avg[i].max < mavg) {
					adc_data.avg[i].max = mavg;
				}
			}
#endif
		}
/*
		uart_putc('a');
		uart_printd(i);
		uart_printd(adc_data.avg[i].sum);
		//				uart_printd(AvgIntValue(adc_data.avg[i]));
		uart_printd(Gradient(adc_data.avg[i]));
		uart_printd(adc_data.avg[i].accel);
		uart_putc('\n');
*/

		adc_data.count++;
		adc_data.count &= 0x7F;
/*

		i = adc_data.count >> 5;	// 32 counts make one measure before channel switching
		if (i > 1) {
			i += 6;
		}
*/
#if defined(SENDLED)
		if (SENDLED > 0) {
			SENDLED = 0;
		} else {
			SENDLED = 1;
		}
#endif
		ADSCR = (1 << 6) | i;		// re-enable interrupt
	}
#endif

#ifdef WITH_KEYBOARD
	if (at_interval(5UL, &keyData.nextPoll)) {	// 4.4 * KEY_DEBOUNCE_CNT ~ 70ms key debouncing
/*
#if defined(SENDLED)
				if (SENDLED > 0) {
					SENDLED = 0;
				} else {
					SENDLED = 1;
				}
#endif
*/
		rb = keyEvent();
		if (rb != keyNoEvent) {
			keybAction();
		}
	}
#endif

	if (at_interval(200UL, &timer0)) {
//		trigger200action();

#ifdef GAPTIMER_TEST
#if defined(SENDLED)
		if (SENDLED > 0) {
			SENDLED = 0;
		} else {
			SENDLED = 1;
		}
#endif
		// 1 tick ~ 8.6809 µs		F_CPU / PRESCALER_VALUE
		setup_gaptimer(MS_TO_GAP(67));  // max time is TIMER_WRAP ms
		do {
		} while ( ! gaptimer_expired() );
#if defined(SENDLED)
		if (SENDLED > 0) {
			SENDLED = 0;
		} else {
			SENDLED = 1;
		}
#endif
#endif
	}

	if (at_interval(1000UL, &timer1)) {
		trigger1000action();
	}

/*
	if (loopCount == 0) {
		uart_putc('+');
		uart_printh(timer_data.now >> 8);
		uart_printh(timer_data.now & 0xFF);
		uart_putc('\t');
		uart_printd( timer_data.now );
		uart_putc('\n');
	}
	loopCount++;
*/
}

int main()
{
	setup();

	do {
#ifdef WITH_SLOTTED_GETTIME
		getTime();
#endif

#if defined(LEDPIN)
		if (LEDPIN > 0) {
			LEDPIN = 0;
		} else {
			LEDPIN = 1;
		}
#endif

		loop();

	} while(1);
}

// #include "nakedISR_hc908.h"


