#include <avr/interrupt.h>
#include "Timer.h"

sTimer8b timer2;
static InterruptOverflow InterruptOverflowHandler;

static void Timer8bModeConfig(T8_MODE mode);
static void Timer8bStart(T8_CLK clk, uint8_t offset);
static void Timer8bStop();
static bool Timer8bIsStart();
static void AttachOverFlow(InterruptOverflow func);
static void DetachOverFlow();

void Timer2Init()
{
	timer2.tccr_a = &TCCR2A;
	timer2.tccr_b = &TCCR2B;
	timer2.timsk_ = &TIMSK2;
	timer2.tifr_ = &TIFR2;
	timer2.tcnt_ = &TCNT2;
	timer2.ocr_a = &OCR2A;

	timer2.Config = &Timer8bModeConfig;
	timer2.Start = &Timer8bStart;
	timer2.Stop = &Timer8bStop;
	timer2.IsStart = &Timer8bIsStart;
	timer2.InterruptOverFlowAttach = &AttachOverFlow;
	timer2.InterruptOverFlowDetach = &DetachOverFlow;
}

static void Timer8bModeConfig(T8_MODE mode)
{
	*timer2.tccr_a = BM_ByteHi(mode);
	*timer2.tccr_b = BM_ByteLow(mode);

	//*timer.tccr_a |= BM_ByteHi(timer8->mode.paramA);
	//*timer.tccr_a |= BM_ByteHi(timer8->mode.paramB);

	*timer2.tccr_b &= 0xF8; //timer off
	//*timer.timsk_ = 0x00; // all interupts off
	*timer2.timsk_ = 0x01; // interrupt overflow on
}

static void Timer8bStart(T8_CLK clk, uint8_t offset)
{
	*timer2.tcnt_ = offset;
	*timer2.tccr_b = (clk != T8_CLK_NULL) 
		? *timer2.tccr_b 
		| clk : *timer2.tccr_b & 0xF8;
}

static void Timer8bStop()
{
	Timer8bStart(T8_CLK_NULL, 0);
}

static bool Timer8bIsStart()
{
	return ((*timer2.tccr_b & 0x07) == T8_CLK_NULL) ? false : true;
}

static void AttachOverFlow(InterruptOverflow func)
{
	*timer2.timsk_ |= 0x01; // interrupt overflow on
	InterruptOverflowHandler = func;
}

static void DetachOverFlow()
{
	*timer2.timsk_ &= ~(1 << 0);
	InterruptOverflowHandler = 0;
}

// Прерывание таймер2 по переполнению 0xFF
ISR(TIMER2_OVF_vect)
{
	if (InterruptOverflowHandler)
	{
		InterruptOverflowHandler();
	}
}