#include "ModbusSlave.h"
#include <string.h>

// Сформировать ответ с ошибкой.
uint8_t* _errorMessage(uint8_t slave_id, uint8_t func, uint8_t err, uint32_t *tx_len);
// Сформировать ответ на запрос с функцией 0x03.
uint8_t* _func3(sSlave *slave, uint8_t *rx,  uint32_t *tx_len);
// Сформировать ответ на запрос с функцией 0x10.
uint8_t* _func10(sSlave *slave, uint8_t *rx,  uint32_t *tx_len);
// Чтение ID (серийного номера)
uint8_t* _func11(sSlave *slave, uint8_t *rx,  uint32_t *tx_len);

//Создать клента
sSlave *ModbusSlaveInit(uint8_t id, uint16_t regCount)
{
	sSlave *slave = (sSlave *)malloc(sizeof(sSlave));

	slave->counter = 0;

	slave->id = id;
	slave->regs = (sRegs16 *)malloc(sizeof(sRegs16));
	slave->regs->offset = 0;
	slave->regs->ppBuf = (uint8_t **)malloc(2 * regCount * sizeof(uint8_t *));
	slave->regs->count = regCount;

	return slave;
}

// Добавить число типа uint8_t в массив регистров
void AddUInt8ToRegs(sSlave *slave, volatile uint8_t *reg)
{
	slave->regs->ppBuf[slave->counter++] = reg;
}

// Добавить число типа uint16_t в массив регистров
void AddUInt16ToRegs(sSlave *slave, volatile uint16_t *val)
{
	AddUInt8ToRegs(slave, ((uint8_t *)val) + 1); //Hi
	AddUInt8ToRegs(slave, (uint8_t *)val); //Low
}

// Добавить число типа uint32_t в массив регистров
void AddUInt32ToRegs(sSlave *slave, uint32_t *val)
{
	AddUInt16ToRegs(slave, ((uint16_t *)val) + 1);
	AddUInt16ToRegs(slave, (uint16_t *)val);
}

// Добавить число типа int в массив регистров
void AddInt32ToRegs(sSlave *slave, int32_t *val)
{
	AddUInt16ToRegs(slave, ((uint16_t *)val) + 1);
	AddUInt16ToRegs(slave, (uint16_t *)val);
}

// Добавить число типа float в массив регистров
void AddFloatToRegs(sSlave *slave, float *val)
{
	AddUInt16ToRegs(slave, ((uint16_t *)val) + 1);
	AddUInt16ToRegs(slave, (uint16_t *)val);
}

// Добавить дополнительную информацию устройства (для ф-ции 0x11)
void AddInfo(sSlave *slave, char* devInfo)
{
	slave->info = devInfo;
}

// Получение и обработка сообщения.
//client - контекст данных
//tx - буфер приемник
//len - длина буфера приеника
//resp - указатель на функцию передачи ответа
uint8_t* Transaction(void *client, uint8_t *rx, uint32_t rx_len, uint32_t* tx_len)
{
	sSlave* slave = (sSlave*)client;

	if (slave == NULL || slave->id != rx[0])
	{
		return NULL;
	}

	uint8_t *tx = NULL;
	*tx_len = 5;

	uint16_t rx_crc16 = BM_WORD(rx[rx_len - 1], rx[rx_len - 2]);
	uint16_t crc16 = GetCRC16(rx, 0, rx_len - 2);

	if (rx_crc16 == crc16)
	{
		switch (rx[1])
		{		
			case ReadHoldingRegisters:
				tx = _func3(slave, rx, tx_len);
				break;

			case PresetMultipleRegisters:
				tx = _func10(slave, rx, tx_len);
				break;

			case DeviceID:
				tx = _func11(slave, rx, tx_len);
				break;

			default:
				tx = _errorMessage(slave->id, rx[1], MB_ERR_NOTFUNC, tx_len);
				break;
		}
	}
	else
	{
		tx = _errorMessage(rx[0], rx[1], MB_ERR_DEFAULT, tx_len);
	}

	crc16 = GetCRC16(tx, 0, *tx_len - 2);

	tx[*tx_len - 2] = BM_ByteLow(crc16);
	tx[*tx_len - 1] = BM_ByteHi(crc16);
	
	return tx;
}

// Получить контрольную сумму переданных\полученных данных.
uint16_t GetCRC16(uint8_t *buf, uint8_t seek, uint32_t bufsize)
{
	uint16_t temp, flag;
	unsigned char i, j;
	temp = 0xFFFF;
	for (i = seek; i < bufsize; i++)
	{
		temp = temp ^ buf[i];
		for (j = 1; j <= 8; j++)
		{
			flag = temp & 0x0001;
			temp >>= 1;
			if (flag)
				temp ^= 0xA001;
		}
	}

	return temp;
}

// Сформировать ответ с ошибкой.
uint8_t* _errorMessage(uint8_t slave_id, uint8_t func, uint8_t err, uint32_t *tx_len)
{
	/*
	Формат: <--
	|адрес устройства|
	| |код команды|
	| | |код ошибки|
	| | | |мл.байт контрольна сумма|
	| | | | |ст.байт контрольна сумма|
	|0|1|2|3|4|
	*/
	*tx_len = 5;
	uint8_t* tx = (uint8_t *)malloc(*tx_len * sizeof(uint8_t));
	
	tx[0] = slave_id;
	tx[1] = func | 0x80;
	tx[2] = err;

	return tx;
}

// Сформировать ответ на запрос с функцией 0x03.
uint8_t* _func3(sSlave *slave, uint8_t *rx,  uint32_t *tx_len)
{
	/*
	Формат запроса: <--
	|адрес устройства|
	| |код команды|
	| | |Адрес первого регистра Hi
	| | | |Адрес первого регистра Low
	| | | | |Количество регистров Hi
	| | | | | |Количество регистров Low
	| | | |	| | |Контрольна сумма Low|
	| | | | | | | |Контрольна сумма Hi|
	|0|1|2|3|4|5|6|7
	*/
	uint8_t* tx = NULL;
	uint16_t offset = BM_WORD(rx[2], rx[3]);
	uint16_t count = BM_WORD(rx[4], rx[5]);

	if (offset >= slave->regs->count)
	{
		tx = _errorMessage(slave->id, rx[1], MB_ERR_NOTOFFSET, tx_len);
	}
	else if (offset + count > slave->regs->offset + slave->regs->count)
	{
		tx = _errorMessage(slave->id, rx[1], MB_ERR_NOTCOUNT, tx_len);
	}
	else
	{
		*tx_len = 3 + count * 2 + 2;
		tx = (uint8_t *)malloc(*tx_len * sizeof(uint8_t));

		uint16_t i = 0;
		uint16_t bcount = count * 2;
		uint16_t boffset = offset * 2;

		tx[0] = slave->id;
		tx[1] = rx[1];
		tx[2] = bcount;		

		while (i < bcount)
		{
			tx[3 + i] = *slave->regs->ppBuf[boffset + i - slave->regs->offset];
			i++;
		}
	}

	return tx;
}

// Сформировать ответ на запрос с функцией 0x10.
uint8_t* _func10(sSlave *slave, uint8_t *rx,  uint32_t *tx_len)
{
	/*
	Формат запроса: <--
	|адрес устройства|
	| |код команды|
	| | |Адрес первого регистра Hi
	| | | |Адрес первого регистра Low
	| | | | |Количество регистров Hi
	| | | | | |Количество регистров Low
	| | | | | | |Количество байт далее
	| | | | | | |Значение Hi
	| | | | | | | |Значение Low
	| | | | | | | | | ...
	| | | |	| | | | | |Контрольна сумма Low|
	| | | | | | | | | |      |Контрольна сумма Hi|
	|0|1|2|3|4|5|6|7|.| n - 2| n - 1
	*/
	uint8_t* tx = NULL;
	uint16_t offset = BM_WORD(rx[2], rx[3]);
	uint16_t count = BM_WORD(rx[4], rx[5]);

	if (offset >= slave->regs->count)
	{
		tx = _errorMessage(slave->id, rx[1], MB_ERR_NOTOFFSET, tx_len);
	}
	else if (offset + count > slave->regs->offset + slave->regs->count)
	{
		tx = _errorMessage(slave->id, rx[1], MB_ERR_NOTCOUNT, tx_len);
	}
	else
	{
		*tx_len = 8;
		tx = (uint8_t *)malloc(*tx_len * sizeof(uint8_t));

		uint16_t i = 0;
		uint16_t bcount = count * 2;
		uint16_t boffset = offset * 2;

		while (i < bcount)
		{
			*slave->regs->ppBuf[boffset + i - slave->regs->offset] = rx[7 + i];
			i++;
		}

		tx[0] = slave->id;
		tx[1] = rx[1];
		tx[2] = BM_ByteHi(offset);
		tx[3] = BM_ByteLow(offset);
		tx[4] = BM_ByteHi(count);
		tx[5] = BM_ByteLow(count);
	}

	return tx;
}

uint8_t* _func11(sSlave *slave, uint8_t *rx,  uint32_t *tx_len)
{
	/*
	Формат запроса: <--
	|адрес устройства|
	| |код команды|
	| | |Контрольна сумма Low|
	| | | |Контрольна сумма Hi|
	|0|1|2|3

	Формат ответа: -->
	|адрес устройства
	| |код команды
	| | |число байт
	| | | |id
	| | | | |индикатор состояния 0x00 - off 0xff - on
	| | | | | |дополнительные данные
	| | | | | |   |контрольна сумма Low|
	| | | | | |   |   |контрольна сумма Hi|
	|0|1|2|3|4|5..|n-2|n-1
	*/

	uint8_t bcount = 2 + strlen(slave->info);
	*tx_len = 5 + bcount;
	uint8_t* tx = (uint8_t *)malloc(*tx_len * sizeof(uint8_t));

	tx[0] = slave->id;
	tx[1] = rx[1];
	tx[2] = bcount;
	tx[3] = slave->id;
	tx[4] = 0xFF;

	uint16_t i = 5;
	while(*slave->info != '\0')
	{
		tx[i++] = *(slave->info++);
	}

	return tx;
}