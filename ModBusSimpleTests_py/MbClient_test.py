from MbClient import *


class TestMbClient():
    def test_read(self):
        print("Arduino atmega2560/ Modbus map (regs_count = 3):")

        client = MbClient(0x01)
        client.connect('COM5', 19200)
        client.read_h3(0, 3)

        print(f"Free RAM = {int.from_bytes(client.resp_bytes[3:5], 'big')} bytes")
        print(f"Sensor temperature = {int.from_bytes(client.resp_bytes[5:7], 'big')}*C")
        print(f"Sensor flag error = {client.resp_bytes[7]}")

        client.get_dev_id()
        print(f"Device Info:")
        print(f"ID: {client.resp_bytes[3]}")
        print(f"Status: {client.resp_bytes[4]}")
        print(f"Extra data: {''.join(chr(e) for e in client.resp_bytes[5:-2])}")

        client.close()


if __name__ == '__main__':
    t = TestMbClient()
    t.test_read()
    input("Press Enter to continue...")
