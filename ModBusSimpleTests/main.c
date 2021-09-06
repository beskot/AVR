#include <stdio.h>
#include <string.h>
#include "../AvrSourceLib/Common/Macros.h"
#include "../ModBusSimple/Sources/Modbus/ModbusSlave.h"

void ClientTransactionFloat(sSlave *slave, Transact);
void ModbusClientFloat(uint8_t *, uint32_t);
void reverse_array(uint8_t a[], int n);
void PrintResponse(char* title, uint8_t *buf, int len);

int main()
{
    int fval = -2; //-60274491;
    sSlave *slave = ModbusSlaveInit(0x01, 2);
    AddInt32ToRegs(slave, &fval);
    ClientTransactionFloat(slave, &Transaction);

    getchar();
    free(slave);

    return 0;
}

void ClientTransactionFloat(sSlave *slave, Transact transact)
{
    uint8_t buf[] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00};

    uint16_t crc16 = GetCRC16(buf, 0, 6);
    buf[6] = BM_ByteLow(crc16);
    buf[7] = BM_ByteHi(crc16);

    transact(slave, buf, 8, &ModbusClientFloat);
}

void ModbusClientFloat(uint8_t *buf, uint32_t len)
{
    PrintResponse("Response(func 3)", buf, len);
    union
    {
        int val;
        uint8_t array[4];
    } u;
    memcpy(u.array, buf + 3, 4);
    reverse_array(u.array, 4);
    printf("fval = %d", u.val);
}

void reverse_array(uint8_t a[], int n)
{
    int i, temp;

    for (i = 0; i < n / 2; i++)
    {
        temp = a[i];
        a[i] = a[n - i - 1];
        a[n - i - 1] = temp;
    }
}

void PrintResponse(char* title, uint8_t *buf, int len)
{
    printf("%s:\n", title);
    int i = 0;
    for (; i < len; i++)
    {
        printf("%02X ", buf[i]);
    }
    printf("\n");
}