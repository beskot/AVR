#ifndef _TIMER_H_
#define _TIMER_H_

#include <avr/io.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "../Common/Macros.h"

typedef void (*InterruptOverflow)(void* object);

typedef uint8_t T8_t;
#define T8_T0 0
#define T8_T2 1

typedef uint8_t T8_MODE;
#define T8_MODE_NORM 0x00
#define T8_MODE_CRR 0x0100
#define T8_MODE_CTC 0x0200
#define T8_MODE_PWM 0x0300

typedef uint16_t T8_CLK;
#define T8_CLK_NULL 0x00
#define T8_CLK_CPU 0x01
#define T8_CLK_8 0x02
#define T8_CLK_64 0x03
#define T8_CLK_256 0x04
#define T8_CLK_1024 0x05
#define T8_CLK_EXT_FLOW 0x06
#define T8_CLK_EXT_RIZEN 0x07

#define T8_NORM_NONE 0x00
#define T8_NORM_TOGGLE 0x4000
#define T8_NORM_CLEAR 0x8000
#define T8_NORM_SET 0xC000

typedef struct
{
	volatile uint8_t *tccr_a;
	volatile uint8_t *tccr_b;
	volatile uint8_t *timsk_;
	volatile uint8_t *tifr_;
	volatile uint8_t *tcnt_;
	volatile uint8_t *ocr_a;

	InterruptOverflow HandlerIOF;
	void* paramIOF;
} sTimer8b;

sTimer8b* Timer8_Init(
	T8_t number, 
	uint8_t* TCCR_A,
 	uint8_t* TCCR_B,
  	uint8_t* TIMSK,
   	uint8_t* TIFR,
    uint8_t* TCNT,
	uint8_t* OCR_A);

void Timer8_SetConfig(sTimer8b* timer8, T8_MODE mode);

void Timer8_Start(sTimer8b* timer8, T8_CLK clk, uint8_t offset);

void Timer8_Stop(sTimer8b* timer8);

bool Timer8_IsStart(sTimer8b* timer8);

void Timer8_InterruptOverFlowAttach(sTimer8b* timer8, InterruptOverflow func);

void Timer8_InterruptOverFlowDetach(sTimer8b* timer8);

#endif