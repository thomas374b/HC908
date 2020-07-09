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

#include "uart_hc908.h"

#ifdef WITH_ADC_CHANNELS
	#include "adc_hc908.h"

	#include "sliding_int_avg.c"
#endif

#ifdef WITH_DIALOGS
	#include "menu_dialogs.h"
#endif

#include "display.h"

unsigned char _sdcc_external_startup()
{
        CONFIG1 |= 1;           // disable COP

        switch_to_pll_clock();

        IMASK1 = 1;                     // disable external interrupts
        BRKSCR = 0;                     // disable break interrupts
//      ADSCR = 0;                      // disable ADC interrupts

//        TACH0IE = 0;                      // disable timer0 interrupts
//        TACH1IE = 0;                      // disable timer1 interrupts
//        TBCH0IE = 0;                      // disable timer0 interrupts
//        TBCH1IE = 0;                      // disable timer1 interrupts


        uart_init();
        SCRIE = 1;		// enable receiver interrupts, must come after uart_init()

        timer_init();
#ifdef WITH_ADC_CHANNELS
        adc_init();
#endif
        return 0;
}


typedef enum {
	menu_item_mask = 0x30,
} menu_masks_e;


#ifdef WITH_STACK_TEST
volatile __data uint8_t __at 0x80 stackTmpH;
volatile __data uint8_t __at 0x81 stackTmpL;
#endif

			uint8_t					global_start_canary;

volatile	timer_data_t 			timer_data;

volatile 	spi_receiver_data_t 	spi_receiver;

volatile 	simple_uart_receiver_t	uart_receiver;

#ifdef WITH_ADC_CHANNELS
volatile 	average8_parms_t 		adc_data;
#endif

			timer_uint_t			timer0;
			timer_uint_t			timer1;
			uint8_t 				i2c_data_buf[I2C_DTATBUF_LEN];
			keyMatrix_data_t 		keyData;
			lcd_data_t 				mcc315_data;
			uint8_t					spiData;
			uint8_t					clockGood;
			uint8_t 				loopCount;
			uint8_t					inputFlags;
			uint8_t					spiFlags;
#ifdef WITH_STACK_TEST
			uint16_t				stackLow;
#endif
			uint8_t					menuIdx;
			uint8_t					menuChoose;

//			sliding_avg_parms_t		timeAvg;
//			uint8_t					strng[32];
//			timer_uint_t			timeout2;
//volatile	uint8_t					relay8_mask;

			uint8_t					global_end_canary;

/*
#define GLOBAL_DATA_SIZE	(   2*sizeof(timer_uint_t) \
								+ I2C_DTATBUF_LEN \
								+ sizeof(timer_data_t) \
								+ sizeof(average8_parms_t) \
								+ sizeof(keyMatrix_data_t) \
								+ sizeof(lcd_data_t) \
								+ sizeof(spi_receiver_data_t) \
								+ sizeof(simple_uart_receiver_t) \
								+ 5 )

//			+ sizeof(sliding_avg_parms_t)
*/

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

#define GLOBAL_DATA_SIZE		(&global_end_canary - &global_start_canary)

inline
void setup()
{
	uint8_t *data = (uint8_t *)&timer0;
	uint8_t i;
	for (i=0; i<GLOBAL_DATA_SIZE; i++) {
		data[i] = 0;
	}

#ifdef WITH_STACK_TEST
	stackLow = 0x360;
#endif

#if defined(LEDPIN_MASK)
	LEDPIN_DDR |= LEDPIN_MASK;
#endif
#if defined(SENDLED_MASK)
	SENDLED_DDR |= SENDLED_MASK;
#endif

	board_init();

	DDRC |= 0xFF;

//	DDRA |= 0x01;
//	DDRF |= 0x02;
//	spiFlags = 0xFE;

	__asm__("cli");

    display_init();	// Timer and Interrupts must be running for this

}


// const uint8_t ports[6] = {PORTA, PORTB, PORTC, PORTD, PORTE, PORTF};






inline void keybAction()
{
	uart_putc('\t');
	uint8_t last = keyData.pLast[keyData.page];
	if (last & keyPress) {
#ifdef WITH_DIALOGS
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
#endif
		uart_putc('p');
	} else {
		if (last & keyRelease) {
			last &= keyAllBits;
#ifdef WITH_DIALOGS
			switch(last) {
				case keyPlus:
				case keyMinus:
					onOffAction(last);
					break;

				case keyA:
				case keyB:
					leftRightAction(last);
					break;

				case keyOK:
				case keyEsc:
					cancelOkAction(last);
					break;
			}
#endif
			uart_putc('r');
		} else {
		}
	}
	uart_printh(last & keyAllBits);
	uart_printh(menuIdx);
	uart_printh(menuChoose);
	uart_putc('\n');
}

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
#ifdef WITH_I2C_CLOCK
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

		if (inputFlags > 0) {
			mcc315lcd_setPropeller(loopCount, spiData);
		} else {
			mcc315lcd_setRotor(loopCount, spiData);
		}
		loopCount++;

		uart_putc('\t');
		uart_printd(timer_data.now);



/*	Compiler produziert Schund
		timer_uint_t ms = TICKS_TO_MS(timer_data.now);
		uart_printd(ms);

		ms = timer_data.now;
		ms *= TIMER_WRAP;
		ms += TICKS_ROUNDUP;
		ms /= OCR_TO_TICKS_DIV;
		uart_printd(ms);

		uart_printd(MS_TO_GAP(1000UL));
		timer_int_t_ delta = (timer_int_t_)timer_data.now;
		delta -= (timer_int_t_)timer1;
		uart_printd(delta);
*/
		uart_printd(timer_data.wrap_count);

#ifdef WITH_I2C_CLOCK


		i = m41t56_read_wa(0, FIXED_I2C_DATA_LEN);

		if (i == 0) {
/*
			for (i=0; i<FIXED_I2C_DATA_LEN; i++) {
				uart_printh(i2c_data_buf[i]);
			}
*/
			if ((i2c_data_buf[0] & 0x80) > 0) {  // if clock is stopped
				i2c_data_buf[0] &= ~0x80;		 // clear stop-bit
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

		} else {
			uart_putc('c');
			uart_printd(i);
		}

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

#ifdef WITH_STACK_TEST
		testStack();

		uart_printd(GLOBAL_DATA_SIZE);

		uart_printhx(stackLow);
#endif




//
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

inline void uartAction()
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
			if ((PORTC & i) > 0) {
				// switch off
				PORTC &= ~i;
			} else {
				// switch on
				PORTC |= i;
			}
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
			if ((PORTA & 1) > 0) {
				// switch off
				PORTA &= ~0x01;
			} else {
				// switch on
				PORTA |= 0x01;
			}

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
				if (spiData & i) {
					spiData &= ~i;
				} else {
					spiData |= i;
				}
			} else {
				if (spiFlags & i) {
					spiFlags &= ~i;		// switch off
				} else {
					spiFlags |= i;		// switch on
				}
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

#if 0
inline
void loop()
{
//	uint8_t strng[32];
//	uint8_t i;

//	uint8_t br;
//	uint8_t m;

//	getTime();

	if (uart_bytesAvailable) {
	}


#if 0
	if (at_interval(50, &timer0)) {

//
#ifdef VARIANT_0
		uart_printd(timer_data.now);
#else
		uint8_t buf[32];
		long2strJK(timer_data.now, 7, 1, buf);
		for (i=0; i<32; i++) {
			if (buf[i] == 0) {
				break;
			}
			uart_putc(buf[i]);
		}
#endif
		uart_printb(PORTA);		// control
		uart_printb(PORTB);		// inputs I5..I12
		uart_printb(PORTC);		// input bits
		uart_printb(PORTD);		// maybe keyboard
		uart_printb(PORTE);		// inputs I2..I4, 2 missing relays ?
//		uart_printb(PORTF);

		uart_printb(PWMOUT);

		uart_putc('\n');
	}
#endif





}
#endif



inline void loop()
{
#ifdef WITH_KEYBOARD
#endif
	uint8_t rb;
	uint8_t i;
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
		intAverage(&adc_data.avg[i], adc_data.raw);

		if (averageStabilizedEvent(&adc_data.avg[i], &adc_data.stabi[i])) {
			// value @ channel i stabilized
			uart_putc('\n');
			uart_putc('a');
			uart_printd(i);
			uart_printd(adc_data.avg[i].sum);
			uart_printd(AvgIntValue(adc_data.avg[i]));
			uart_printd(Gradient(adc_data.avg[i]));
			uart_putc('\n');
		}

		adc_data.count++;
		adc_data.count &= 0x7F;

		i = adc_data.count >> 5;	// 32 counts make one measure before channel switching
		if (i > 1) {
			i += 6;
		}
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


