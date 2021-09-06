import serial
import time

'''Modbus client class'''


class MbClient:

    def __init__(self, id):
        self.resp_bytes = []
        self.__dev_addr = id

    '''Serail open'''

    def connect(self, port, boundrate):
        self.__ser = serial.Serial(port, boundrate)
        time.sleep(3)

    '''Serial close'''

    def close(self):
        if(self.__ser.isOpen()):
            self.__ser.close()

    '''Modbus func 0x03'''

    def read_h3(self, start, count):
        self.__execute([self.__dev_addr, 0x03, start >> 8, start &
                       0x00FF, count >> 8, count & 0x00FF])

    '''Modbus func 0x10'''

    def write_h10(self, start, data):
        self.__execute([self.__dev_addr, 0x10, start >> 8, start &
                        0x00FF, 0x00, 0x01, data >> 8, data & 0x00FF])

    def __execute(self, buffer):
        try:
            self.resp_bytes.clear()
            buffer.extend(self.__get_crc16(buffer))
            self.__ser.write(buffer)
            time.sleep(0.1)
            while self.__ser.inWaiting() > 0:
                self.resp_bytes.extend(self.__ser.read())
            print([hex(x) for x in self.resp_bytes])
        except Exception:
            print(f"Exeption: {Exception}")

    '''Modbus CRC16'''

    def __get_crc16(self, arraybyte):
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
