#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include "../AvrSourceLib/Uart/Uart.h"
#include "./Modbus/ModbusSlave.h"

const char *device = "_wifi_";
uint8_t val8 = 0x50;
uint16_t val16 = 0x1234;
uint32_t val32 = 0x44332211;
float val_f = -5.65f;

int main(void)
{
    DDRB = 0xFF;
    PORTB = 0x00;
    TCCR0A = 0x83;
    TCCR0B = 0x03;
    OCR0A = 0x10;
    //cli();
    sei();

    sSlave *slave = ModbusSlaveInit(0x01, 1);

    AddUInt8ToRegs(slave, &OCR0A);  //0.5 reg / 1b
    AddUInt8ToRegs(slave, &val8);   //0.5 reg / 1b

    // AddFloatToRegs(slave, &val_f);  //2 reg /4b
    // AddUInt32ToRegs(slave, &val32); //2 reg /4b
    // AddUInt8ToRegs(slave, &TCCR0A); //0.5 reg / 1b
    // AddUInt8ToRegs(slave, &TCCR0B); //0.5 reg / 1b
    // AddUInt8ToRegs(slave, &OCR0A);  //0.5 reg / 1b
    // AddUInt8ToRegs(slave, &val8);   //0.5 reg / 1b
    // AddUInt16ToRegs(slave, &val16); //1 reg / 2b
    // AddUInt8ToRegs(slave, &OCR0A);  //0.5 reg / 1b
    // AddUInt8ToRegs(slave, &val8);   //0.5 reg / 1b

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