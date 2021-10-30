#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>
#include "../AvrSourceLib/Uart/Uart.h"
#include "../AvrSourceLib/Common/Debug.h"
#include "../ModBusSimple/Sources/Modbus/ModbusSlave.h"
#include "./ds18b20/ds18b20.h"

int16_t sensor_t_value = 0;
uint8_t sensor_t_err = 0;
int16_t freeRam = 0;
uint8_t dummy = 0xff;

int main(void)
{
    DDRB = 0xFF;

    //cli();
    sei();
    
    sSlave *slave = ModbusSlaveInit(0x01, 3);
    AddUInt16ToRegs(slave, &freeRam);
    AddUInt16ToRegs(slave, &sensor_t_value);
    AddUInt8ToRegs(slave, &sensor_t_err);
    AddUInt8ToRegs(slave, &dummy);
    
    sSerialPort* uart0 = UartInit(SERIALPORT_NUM_0, &UBRR0H, &UBRR0L, &UCSR0A, &UCSR0B, &UCSR0C, &UDR0);
    UartBegin(uart0, SERIALPORT_BR_19200, SERIALPORT_DB_8, SERIALPORT_PRT_NONE, SERIALPORT_SB_ONE);
    SetTransmitter(uart0, Transaction);
    uart0->client = (void *)slave;

    while (1)
    {
        freeRam = getFreeRam();
        //Start conversion (without ROM matching)
        //ARDUINO ATMEGA2560 B7 - 12 Pin
        ds18b20convert(&PORTB, &DDRB, &PINB, (1 << 6), NULL);

        //Delay (sensor needs time to perform conversion)
        _delay_ms(1000);

        //Read temperature (without ROM matching)
        sensor_t_err = ds18b20read(&PORTB, &DDRB, &PINB, (1 << 6), NULL, &sensor_t_value);

        sensor_t_value /= 16;
      
        _delay_ms(10000);
    }

    return 0;
}