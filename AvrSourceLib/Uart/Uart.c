#include <avr/interrupt.h>
#include "Uart.h"
#include "../Timer/Timer.h"

sSerialPort serial;
static uint8_t buffer[BUFSIZE];
static uint32_t size = 0;

static Transact Transaction;

static void _uartBegin(SERIALPORT_BR, SERIALPORT_DB, SERIALPORT_PRT, SERIALPORT_SB);
static void _uartWriteByte(uint8_t);
static uint8_t _uartReadByte();
static void _uartWrite(uint8_t *, uint32_t);
static uint32_t _uartRead(uint8_t *);

static void _receiveCompleteHandler(Transact func);
static void _uartReadComplete();

void Uart0Init()
{
	serial.UBRR_H = &UBRR0H;
	serial.UBRR_L = &UBRR0L;
	serial.UCSR_A = &UCSR0A;
	serial.UCSR_B = &UCSR0B;
	serial.UCSR_C = &UCSR0C;
	serial.UDR_ = &UDR0;

	serial.Begin = &_uartBegin;
	serial.SendBytes = &_uartWrite;
	serial.ReceiveBytes = &_uartRead;
	serial.SetTransaction = &_receiveCompleteHandler;

	Timer2Init();
	timer2.Config(T8_MODE_NORM);
	timer2.InterruptOverFlowAttach(&_uartReadComplete);
}

static void _uartBegin(SERIALPORT_BR br, SERIALPORT_DB b, SERIALPORT_PRT p, SERIALPORT_SB sb)
{
	uint16_t baundrate = ((F_CPU / br) / 16UL) - 1;
	*serial.UBRR_H = (uint8_t)(baundrate >> 8);
	*serial.UBRR_L = (uint8_t)(baundrate & 0x00ff);

	*serial.UCSR_B = BM_ByteHi(b);
	*serial.UCSR_C |= BM_ByteLow(b);

	*serial.UCSR_C |= p;
	*serial.UCSR_C |= sb;

	//*serial.UCSR_B |= SERIALPORT_CTRL_EN_INT_RX;
	//*serial.UCSR_B &= ~SERIALPORT_CTRL_EN_INT_RX;
	//*serial->UCSR_B |= SERIALPORT_CTRL_EN_TX;
}

static void _uartWriteByte(uint8_t data)
{
	//Ожидать пока приемник не будет готов
	while (!(*serial.UCSR_A & (1 << UDRE0))) { }
	//Записать данные в буфер
	*serial.UDR_ = data;
}

static uint8_t _uartReadByte()
{
	//Ожидать пока данные не будут получены
	while (!(*serial.UCSR_A & (1 << RXC0))) { }
	//Получить данные из буфера приемника
	return *serial.UDR_;
}

static void _uartWrite(uint8_t *buf, uint32_t len)
{
	while (len--)
	{
		_uartWriteByte(*buf++);
	}
}

static uint32_t _uartRead(uint8_t *buf)
{
	if (timer2.IsStart())
	{
		*timer2.tcnt_ = 0;
	}
	else
	{
		timer2.Start(T8_CLK_1024, 0);
		size = 0;
	}
	*(buf++) = _uartReadByte(serial);
	size++;

	return size;
}

static void _uartReadComplete()
{
	timer2.Stop();

	*serial.UCSR_B &= ~(SERIALPORT_CTRL_EN_RX | SERIALPORT_CTRL_EN_INT_RX);
	*serial.UCSR_B |= SERIALPORT_CTRL_EN_TX;

	if (Transaction)
	{
		Transaction(serial.client, buffer, size, &_uartWrite);
	}

	*serial.UCSR_B &= ~SERIALPORT_CTRL_EN_TX;
	*serial.UCSR_B |= SERIALPORT_CTRL_EN_RX | SERIALPORT_CTRL_EN_INT_RX;
}

static void _receiveCompleteHandler(Transact func)
{
	*serial.UCSR_B &= ~SERIALPORT_CTRL_EN_TX;
	*serial.UCSR_B |= SERIALPORT_CTRL_EN_RX | SERIALPORT_CTRL_EN_INT_RX;
	//*serial.UCSR_B |= SERIALPORT_CTRL_EN_TX;
	Transaction = func;
}

// Прерывание по приему
ISR(USART0_RX_vect)
{
	if (timer2.IsStart())
	{
		*timer2.tcnt_ = 0;
	}
	else
	{
		size = 0;
		timer2.Start(T8_CLK_1024, 0);
	}
	buffer[size++] = _uartReadByte(serial);
}