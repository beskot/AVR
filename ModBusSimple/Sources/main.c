#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include "../AvrSourceLib/Uart/Uart.h"
#include "./Modbus/ModbusSlave.h"

uint8_t val8 = 0x50;

int main(void)
{
    DDRB = 0xFF;
    PORTB = 0x00;
    TCCR0A = 0x83;
    TCCR0B = 0x03;
    OCR0A = 0x10;
    //cli();
    sei();

    sSlave *slave = ModbusSlaveInit(0x01);

    AddInfo(slave, "Modbus ver 1.0.0.0");
    AddUInt8ToRegs(slave, &OCR0A);
    AddUInt8ToRegs(slave, &val8);

    sSerialPort* uart0 = UartInit(SERIALPORT_NUM_0, &UBRR0H, &UBRR0L, &UCSR0A, &UCSR0B, &UCSR0C, &UDR0);
    UartBegin(uart0, SERIALPORT_BR_19200, SERIALPORT_DB_8, SERIALPORT_PRT_NONE, SERIALPORT_SB_ONE);
    uart0->client = (void*)slave;
    SetTransmitter(uart0, Transaction);
    
    while (1)
    {
        // _delay_ms(2000);
    }

    return 0;
}