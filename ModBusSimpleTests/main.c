#include <stdio.h>
#include <string.h>
#include "../AvrSourceLib/Common/Macros.h"
#include "../ModBusSimple/Sources/Modbus/ModbusSlave.h"

void ClientTransactionFloat(sSlave *slave, Transact);
void reverse_array(uint8_t a[], int n);
void PrintResponse(char* title, uint8_t *buf, int len);

void CvectorTest()
{
    int k = 210;
    cvector* vector = cvector_init(sizeof(int));
    cvector_push(vector, &k);
    *(int*)cvector_get(vector, 0) = 14;
}

void ModbusTest()
{
    uint16_t fval = 0x7788; //-60274491;
    sSlave *slave = ModbusSlaveInit(0x01);
    AddUInt16ToRegs(slave, &fval);
   
    ClientTransactionFloat(slave, &Transaction);

    getchar();
    free(slave);
}

int main()
{
    CvectorTest();
    ModbusTest();

    return 0;
}

void ClientTransactionFloat(sSlave *slave, Transact transact)
{
    uint8_t buf[] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00};

    uint16_t crc16 = GetCRC16(buf, 0, 6);
    buf[6] = BM_ByteLow(crc16);
    buf[7] = BM_ByteHi(crc16);

    uint32_t tx_len = 0;
    uint8_t *tx_buf = transact(slave, buf, 8, &tx_len);

    if (tx_buf == NULL)
    {
        return;
    }
    
    PrintResponse("Response(func 3)", tx_buf, tx_len);
    union
    {
        int val;
        uint8_t array[4];
    } u;
    memcpy(u.array, tx_buf + 3, 4);
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