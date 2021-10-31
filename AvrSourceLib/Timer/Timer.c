#include <avr/interrupt.h>
#include "Timer.h"

sTimer8b* timers8[2];

sTimer8b* Timer8_Init(
	T8_t number,
	uint8_t* TCCR_A,
	uint8_t* TCCR_B,
	uint8_t* TIMSK,
	uint8_t* TIFR,
	uint8_t* TCNT,
	uint8_t* OCR_A)
{
	sTimer8b * timer = (sTimer8b *)malloc(sizeof(sTimer8b));
	timer->tccr_a = TCCR_A;
	timer->tccr_b = TCCR_B;
	timer->timsk_ = TIMSK;
	timer->tifr_ = TIFR;
	timer->tcnt_ = TCNT;
	timer->ocr_a = OCR_A;

	timers8[number] = timer;

	return timer;
}

void Timer8_SetConfig(sTimer8b* timer8, T8_MODE mode)
{
	*timer8->tccr_a = BM_ByteHi(mode);
	*timer8->tccr_b = BM_ByteLow(mode);

	//*timer.tccr_a |= BM_ByteHi(timer8->mode.paramA);
	//*timer.tccr_a |= BM_ByteHi(timer8->mode.paramB);

	*timer8->tccr_b &= 0xF8; //timer off
	//*timer.timsk_ = 0x00; // all interupts off
	*timer8->timsk_ = 0x01; // interrupt overflow on
}

void Timer8_Start(sTimer8b* timer8, T8_CLK clk, uint8_t offset)
{
	*timer8->tcnt_ = offset;
	*timer8->tccr_b = (clk != T8_CLK_NULL) 
		? *timer8->tccr_b | clk
		: *timer8->tccr_b & 0xF8;
}

void Timer8_Stop(sTimer8b* timer8)
{
	Timer8_Start(timer8, T8_CLK_NULL, 0);
}

bool Timer8_IsStart(sTimer8b* timer8)
{
	return (*timer8->tccr_b & 0x07) == T8_CLK_NULL ? false : true;
}

void Timer8_InterruptOverFlowAttach(sTimer8b* timer8, InterruptOverflow func)
{
	*timer8->timsk_ |= 0x01; // interrupt overflow on
	timer8->HandlerIOF = func;
}

void Timer8_InterruptOverFlowDetach(sTimer8b* timer8)
{
	*timer8->timsk_ &= ~(1 << 0);
	timer8->HandlerIOF = 0;
}

// Прерывание таймер2 по переполнению 0xFF
ISR(TIMER2_OVF_vect)
{
	if (timers8[1] != NULL 
		&& timers8[1]->HandlerIOF != NULL)
	{
		timers8[1]->HandlerIOF(timers8[1]->paramIOF);
	}
}