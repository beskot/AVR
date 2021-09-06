CC=gcc
CFLAGS=-g -I ../AvrSourceLib
BIN_DIR = ./Build
DEPS = ../AvrSourceLib/Common/Macros.h ../ModBusSimple/Sources/Modbus/ModbusSlave.h

$(shell mkdir -p $(BIN_DIR))

$(BIN_DIR)/ModbusSlave.o: ../ModBusSimple/Sources/Modbus/ModbusSlave.c  $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(BIN_DIR)/main.o: main.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

main: $(BIN_DIR)/main.o $(BIN_DIR)/ModbusSlave.o
	$(CC) -o $(BIN_DIR)/$@ $^ $(CFLAGS)

clean:
	if [ -d "$BIN_DIR" ]; then rm -Rf $BIN_DIR; fi

all: clean main