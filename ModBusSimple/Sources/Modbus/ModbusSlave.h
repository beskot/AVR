#ifndef _MODBUSSLAVE_H_
#define _MODBUSSLAVE_H_

#include <stdint.h>
#include <stdlib.h>
#include "../AvrSourceLib/Common/Macros.h"

// Команды
#define ReadHoldingRegisters 0x03
#define PresetMultipleRegisters 0x10
#define DeviceID 0x11

// Ошибки
/*
	список кодов ошибок:
	01 — функция не поддерживается. Это значит, что, возможно, функция не стандартная или просто не реализована конкретно
	 	в этом устройстве.
	02 — запрошенная область памяти не доступна. Каждое устройство содержит определённое количество данных определённого типа.
	 	Например, в устройстве доступно 100 регистров общего назначения. Если при этом запросить чтение 101 регистров, то возникнет
	  	ошибка 02.
	03 — функция не поддерживает запрошенное количество данных. Например, функция Read Holding Registers позволяет считывать
	 	от 1 до 2000 регистров общего назначения. Поэтому, даже если в подчинённом устройстве доступно для чтения 10 000 регистров,
	  	при запросе более 2000 с помощью данной функции возникает эта ошибка.
	04 — функция выполнена с ошибкой. Этот код отсылается в случае, если ошибка не относится к трем предыдущим кодам.
*/
#define MB_ERR_NOTFUNC 1
#define MB_ERR_NOTOFFSET 2
#define MB_ERR_NOTCOUNT 3
#define MB_ERR_DEFAULT 4

//Интерфейс приема\передачи
typedef uint8_t* (*Transact)(void* client, uint8_t* rx, uint32_t rxLen, uint32_t* txLen);

typedef struct
{
	uint16_t offset;
	uint16_t count;
	uint8_t **ppBuf;
} sRegs16;

typedef struct
{
	uint8_t id;
	sRegs16 *regs;
	uint8_t counter;
	char* info;
} sSlave;

sSlave *ModbusSlaveInit(uint8_t id, uint16_t bufSize);
uint8_t* Transaction(void *client, uint8_t *rx, uint32_t rx_len, uint32_t* tx_len);
void AddUInt8ToRegs(sSlave *slave, volatile uint8_t *reg);
void AddUInt16ToRegs(sSlave *slave, volatile uint16_t *val);
void AddUInt32ToRegs(sSlave *slave, uint32_t *val);
void AddInt32ToRegs(sSlave *slave, int32_t *val);
void AddFloatToRegs(sSlave *slave, float *val);
uint16_t GetCRC16(uint8_t *, uint8_t, uint32_t);
void AddInfo(sSlave *slave, char* devInfo);

#endif