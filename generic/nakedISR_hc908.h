/*
 * nakedISR_hc908.h
 *
 *  Created on: 13.06.2020
 *      Author: pantec
 */

#ifndef NAKEDISR_HC908_H_
#define NAKEDISR_HC908_H_




void SWI_isr(void) __interrupt(_isrNo_SWI) __naked
{
	__asm
		rti
	__endasm;
}

void extIRQ_isr(void) __interrupt(_isrNo_IRQ) __naked
{
	__asm
		bset		#2,*_INTSCR		; write ACKI, extern IRQ served
		rti
	__endasm;
}

//			2	unused


void Tim0_isr(void) __interrupt(_isrNo_TIM0) __naked
{
	__asm
		bclr		#7,*_T1SC0		; interrupt is served
		rti
	__endasm;
}

void Tim1_isr(void) __interrupt(_isrNo_TIM1) __naked
{
	__asm
		bclr		#7,*_T1SC1		; interrupt is served
		rti
	__endasm;
}

#if !defined(HAVE_TOVF_ISR)
void TimOvf_isr(void) __interrupt(_isrNo_TIMOVF) __naked	// this is the correct number in this weird compiler
{
	__asm
		bclr		#7,*_T1SC		; interrupt is served
		rti
	__endasm;
}
#endif


void KEYB_isr(void) __interrupt(_isrNo_KEYB) __naked
{
	__asm
		bset		#2,*_KBSCR			; write ACKK, keyboard IRQ served
		rti
	__endasm;
}

#if defined(_MC68HC908MR32_H) || defined(_MC68HC908JKJL_H)
#if !defined(HAVE_ADC_ISR)
void ADC_isr(void) __interrupt(_isrNo_ADC)  __naked
{
	__asm
		lda			*_ADR				; interrupt is served
		rti
	__endasm;
}
#endif
#endif // header for CPU with ADC included



#endif /* NAKEDISR_HC908_H_ */
