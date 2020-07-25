/**
 *
 *	very simple example project to log data from the ADC channel 0 to UART
 *
 */


#include "compiler_lib.h"

#include "board.h"

#include "uart_printd.h"

#include "sliding_int_avg.c"


unsigned char _sdcc_external_startup()
{
	CONFIG1 |= 1;           // disable COP

	IMASK1 = 1;             // disable external interrupts
	BRKSCR = 0;             // disable break interrupts

	board_init();

	return 0;
}

inline
void setup()
{
}

inline
void loop()
{
#ifdef WITH_NEW_STABILIZER_CODE
	uint8_t b = 0;
#endif
	if (AIEN == 0) {	// the interrupt handler has disabled interrupts when value was ready
		intAverage(&adc_data.avg, adc_data.raw);
#ifdef WITH_NEW_STABILIZER_CODE
		b = averageStabilizedEvent(&adc_data.avg, &adc_data.stabi);
		if (b) {
#endif
#ifdef WITH_TIMER_SYSCLOCK
			getTime();
			uart_printd(timer_data.now);
#endif
			uart_printd(adc_data.count);
			uart_printd(adc_data.raw);
			uart_printd(AvgIntValue(adc_data.avg));
//			uart_printd(divide(adc_data.avg.gradAvg));
#ifdef WITH_AVG_ACCELERATION
//			uart_printd(divide(adc_data.avg.accelAvg));
#endif
#ifdef WITH_NEW_STABILIZER_CODE
			uart_printd(divide(adc_data.stabi.rms));
			uart_printd(adc_data.stabi.threshold);
#ifdef WITH_FREEZING_AVG
			uart_printd(adc_data.stabi.freezer.sum / adc_data.stabi.freezer.cnt);
#endif
			uart_printd(adc_data.stabi.isBraking);
			uart_printd(adc_data.stabi.isStable);
#endif
			uart_putc('\n');
#ifdef WITH_NEW_STABILIZER_CODE
		}
#endif
		adc_data.count++;
		ADSCR = (1 << 6) | DEFAULT_ADC_CHANNEL;		// re-enable interrupt
	}

	// could go to sleep here, timer should continue to count
	// ADC and Timer interrupts must be wakeup sources
}

void main()
{
	setup();
	while(1) {
		loop();
	}
}







