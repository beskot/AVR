import serial
import time

class MbClient:
    '''Класс modbus master (клиент)

    Attributes
    ----------
    resp_bytes : list
        массив байт, содержащий ответ ведомого устройства на запрос
    __dev_addr : byte
        Id ведомого устройства
    '''

    def __init__(self, id):
        self.resp_bytes = []
        self.__dev_addr = id

    def connect(self, port, baudrate) -> None:
        '''Метод для подключения к ведомому устройству по последовательному порту

        Parameters
        ----------
        port : int
            Номер последовательного порта
        baudrate : int
            Символьная скорость
        '''
        self.__ser = serial.Serial(port, baudrate)
        time.sleep(3)

    def close(self) -> None:
        '''Метод закрытия соединения'''

        if(self.__ser.isOpen()):
            self.__ser.close()

    def read_h3(self, start, count) -> None:
        '''Чтение состояния Holding Registers (Функция 03H)

        Parameters
        ----------
        start : int
            Адрес регистра
        count : int
            Количество регистров
        '''

        self.__execute([self.__dev_addr, 0x03, start >> 8, start &
                       0x00FF, count >> 8, count & 0x00FF])

    def write_one_h10(self, start, data) -> None:
        '''Установка одного Holding Register (Функция 10H)

        Parameters
        ----------
        start : int
            Адрес регистра
        data : int
            Данные
        '''

        self.__execute([self.__dev_addr, 0x10, start >> 8, start &
                        0x00FF, 0x00, 0x01, data >> 8, data & 0x00FF])

    def __execute(self, buffer) -> None:
        '''Закрытый метод для записи\чтения данных в ведомое устройство

        Parameters
        ----------
        buffer : list
            Запрос к ведомому устройству (в виде массива байт)
        '''

        try:
            self.resp_bytes.clear()
            buffer.extend(self.__get_crc16(buffer))
            self.__ser.write(buffer)
            time.sleep(0.1)
            while self.__ser.inWaiting() > 0:
                self.resp_bytes.extend(self.__ser.read())
            print([hex(x) for x in self.resp_bytes])
        except Exception as ex:
            print(f"Exeption: {ex}")

    def __get_crc16(self, arraybyte) -> tuple:
        '''Закрытый метод для расчета контрольной суммы

        Parameters
        ----------
        arraybyte : arraybyte
            Запрос к ведомому устройству (в виде массива байт, но без контрольной суммы)
        '''

        polynom = 0xa001  # полином modbus
        crcfull = 0xffff
        crcH, crcL = 0xff, 0xff
        flag = False
        for byte in arraybyte:
            crcfull ^= byte
            for i in range(8):
                flag = crcfull & 0x0001
                crcfull = crcfull >> 1
                if (flag == 1):
                    crcfull ^= polynom
        crcH = (crcfull >> 8) & 0xff
        crcL = crcfull & 0xff
        return (crcL, crcH)
