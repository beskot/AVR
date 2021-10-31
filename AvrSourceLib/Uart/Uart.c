#include <avr/interrupt.h>
#include "Uart.h"
#include "../Timer/Timer.h"

static sTimer8b* timer;

static sSerialPort* serials[4];
static uint8_t buffer[BUFSIZE];
static uint32_t rx_size = 0;
static uint32_t tx_size = 0;

static Transact Transaction;
static void _uartReadComplete();
static void _usart_rx_vect(sSerialPort* serial);

sSerialPort* UartInit(
	SERIALPORT_NUM number,
	uint8_t* UBRR_H,
	uint8_t* UBRR_L,
	uint8_t* UCSR_A,
	uint8_t* UCSR_B,
	uint8_t* UCSR_C,
	uint8_t* UDR)
{
	sSerialPort * serial = (sSerialPort *)malloc(sizeof(sSerialPort));
	serial->UBRR_H = UBRR_H;
	serial->UBRR_L = UBRR_L;
	serial->UCSR_A = UCSR_A;
	serial->UCSR_B = UCSR_B;
	serial->UCSR_C = UCSR_C;
	serial->UDR_ = UDR;

	timer = Timer8_Init(T8_T2, &TCCR2A, &TCCR2B, &TIMSK2, &TIFR2, &TCNT2, &OCR2A);
	Timer8_SetConfig(timer, T8_MODE_NORM);
	Timer8_InterruptOverFlowAttach(timer, &_uartReadComplete);

	serials[number] = serial;
	return serial;
}

void UartBegin(
	sSerialPort* serial,
	SERIALPORT_BR br,
	SERIALPORT_DB b,
	SERIALPORT_PRT p,
	SERIALPORT_SB sb)
{
	uint16_t baundrate = ((F_CPU / br) / 16UL) - 1;
	*serial->UBRR_H = (uint8_t)(baundrate >> 8);
	*serial->UBRR_L = (uint8_t)(baundrate & 0x00ff);

	*serial->UCSR_B = BM_ByteHi(b);
	*serial->UCSR_C |= BM_ByteLow(b);

	*serial->UCSR_C |= p;
	*serial->UCSR_C |= sb;
}

void UartWriteByte(sSerialPort* serial, uint8_t data)
{
	while (!(*serial->UCSR_A & (1 << 5))) { }
	*serial->UDR_ = data;
}

uint8_t UartReadByte(sSerialPort* serial)
{
	while (!(*serial->UCSR_A & (1 << 7))) { }
	return *serial->UDR_;
}

void UartWrite(sSerialPort* serial, uint8_t *buf, uint32_t len)
{
	while (len--)
	{
		UartWriteByte(serial, *buf++);
	}
}

uint32_t UartRead(sSerialPort* serial, uint8_t *buf)
{
	if (Timer8_IsStart(timer))
	{
		*timer->tcnt_ = 0;
	}
	else
	{
		Timer8_Start(timer, T8_CLK_1024, 0);
		rx_size = 0;
	}
	*(buf++) = UartReadByte(serial);
	rx_size++;

	return rx_size;
}

static void _uartReadComplete(void* object)
{
	sSerialPort* ser = (sSerialPort*) object;
	Timer8_Stop(timer);

	*ser->UCSR_B &= ~(SERIALPORT_CTRL_EN_RX | SERIALPORT_CTRL_EN_INT_RX);
	*ser->UCSR_B |= SERIALPORT_CTRL_EN_TX;

	if (Transaction)
	{
		uint8_t* tx_buf = Transaction(ser->client, buffer, rx_size, &tx_size);
		if (tx_buf != NULL && tx_size > 0)
		{
			UartWrite(ser, tx_buf, tx_size);
			free(tx_buf);
		}
	}

	*ser->UCSR_B &= ~SERIALPORT_CTRL_EN_TX;
	*ser->UCSR_B |= SERIALPORT_CTRL_EN_RX | SERIALPORT_CTRL_EN_INT_RX;
}

void SetTransmitter(sSerialPort* serial, Transact func)
{
	*serial->UCSR_B &= ~SERIALPORT_CTRL_EN_TX;
	*serial->UCSR_B |= SERIALPORT_CTRL_EN_RX | SERIALPORT_CTRL_EN_INT_RX;
	Transaction = func;
}

static void _usart_rx_vect(sSerialPort* serial)
{
	if (Timer8_IsStart(timer))
	{
		*timer->tcnt_ = 0;
	}
	else
	{
		timer->paramIOF = (void*)serial;
		rx_size = 0;
		Timer8_Start(timer, T8_CLK_1024, 0);		
	}
	buffer[rx_size++] = UartReadByte(serial);
}

// Прерывание по приему
ISR(USART0_RX_vect)
{
	_usart_rx_vect(serials[0]);
}

ISR(USART1_RX_vect)
{
	_usart_rx_vect(serials[1]);
}

ISR(USART2_RX_vect)
{
	_usart_rx_vect(serials[2]);
}

ISR(USART3_RX_vect)
{
	_usart_rx_vect(serials[3]);
}