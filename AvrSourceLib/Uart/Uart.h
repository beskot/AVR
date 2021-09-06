#ifndef _UART_H_
#define _UART_H

#include <stdint.h>
#include <avr/io.h>
#include <stdlib.h>
#include "../Common/Macros.h"

typedef void (*pRecv)(uint8_t* buf, uint32_t len);
typedef void (*pResp)(uint8_t *buf, uint32_t len);
typedef void (*Transact)(void* client, uint8_t*, uint32_t, pResp resp);

#define BUFSIZE 256

typedef uint8_t SERIALPORT_NUM;
#define SERIALPORT_NUM_0 0
#define SERIALPORT_NUM_1 1
#define SERIALPORT_NUM_2 2
#define SERIALPORT_NUM_3 3
#define SERIALPORT_NUM_4 4

typedef uint32_t SERIALPORT_BR;
#define SERIALPORT_BR_1200		1200
#define SERIALPORT_BR_2400		2400
#define SERIALPORT_BR_4800		4800
#define SERIALPORT_BR_9600		9600
#define SERIALPORT_BR_14400		14400
#define SERIALPORT_BR_19200		19200
#define SERIALPORT_BR_38400		38400
#define SERIALPORT_BR_57600		57600
#define SERIALPORT_BR_115200	115200
#define SERIALPORT_BR_230400	230400
#define SERIALPORT_BR_260800	260800
#define SERIALPORT_BR_921600	921600

typedef uint16_t SERIALPORT_DB;
#define SERIALPORT_DB_5 0x00
#define SERIALPORT_DB_6 0x02
#define SERIALPORT_DB_7 0x04
#define SERIALPORT_DB_8 0x06
#define SERIALPORT_DB_9 0x0406

typedef uint8_t SERIALPORT_PRT;
#define SERIALPORT_PRT_NONE 0x00
#define SERIALPORT_PRT_EVEN 0x20
#define SERIALPORT_PRT_ODD	0x30

typedef uint8_t SERIALPORT_SB;
#define SERIALPORT_SB_ONE 0x00
#define SERIALPORT_SB_TWO 0x08

typedef uint8_t SERIALPORT_CTRL;
#define SERIALPORT_CTRL_EN_TX		0x08
#define SERIALPORT_CTRL_EN_RX		0x10
#define SERIALPORT_CTRL_EN_INT_UDR	0x20
#define SERIALPORT_CTRL_EN_INT_TX	0x40
#define SERIALPORT_CTRL_EN_INT_RX	0x80

typedef struct {
	volatile uint8_t* UBRR_H;
	volatile uint8_t* UBRR_L;
	volatile uint8_t* UCSR_A;
	volatile uint8_t* UCSR_B;
	volatile uint8_t* UCSR_C;
	volatile uint8_t* UDR_;

	void (*Begin)(SERIALPORT_BR, SERIALPORT_DB, SERIALPORT_PRT, SERIALPORT_SB);
	void (*SendBytes)(uint8_t* buf, uint32_t count);
	uint32_t (*ReceiveBytes)(uint8_t* buf);
	void (*SetTransaction)(Transact);
	void* client;
}sSerialPort;

void Uart0Init();
extern sSerialPort serial;

#endif