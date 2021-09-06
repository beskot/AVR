CC = avr-gcc -I $(AVR_SOURCE_LIB)

_TARGET = $@
_DEPEND = $<
_DEPEND_ALL = $^

TARGET_BIN = $(BIN_DIR)/modbus_sensor_t
AVR_SOURCE_LIB = ../AvrSourceLib
BIN_DIR = ./Build

SOURCE_H = \
$(AVR_SOURCE_LIB)/Common/Macros.h \
$(AVR_SOURCE_LIB)/Timer/Timer.h \
$(AVR_SOURCE_LIB)/Uart/Uart.h \
$(AVR_SOURCE_LIB)/Common/Debug.h \
../ModBusSimple/Sources/Modbus/ModbusSlave.h \
Sources/ds18b20/ds18b20.h \
Sources/ds18b20/onewire.h \
Sources/ds18b20/romsearch.h

SOURCE_C = \
$(AVR_SOURCE_LIB)/Common/Debug.c \
$(AVR_SOURCE_LIB)/Timer/Timer.c \
$(AVR_SOURCE_LIB)/Uart/Uart.c \
../ModBusSimple/Sources/Modbus/ModbusSlave.c \
Sources/ds18b20/ds18b20.c \
Sources/ds18b20/onewire.c \
Sources/ds18b20/romsearch.c \
main.c

OBJECTS = $(addprefix $(BIN_DIR)/, $(notdir $(SOURCE_C:.c=.o)))
# OBJECTS =  $(SOURCE_C:.c=.o)
# OBJECTS = $(addprefix $(BIN_DIR)/, $(SOURCE_C:.c=.o))
# OBJECTS = $(addprefix $(BIN_DIR)/, $(patsubst %.c,%.o,$(notdir $(SOURCE_C))))

#Контроллер, установленный на плате. Может быть другим, например atmega328 
DEVICE = atmega2560

#Последовательный порт
PORT = COM5

#Символьная скорость
BR = 115200

#Тактовая частота 16 МГц 
CLOCK = 16000000

#Тип программатора
PROGRAMMER_TYPE = stk500v2

#Команда запуска avrdude. Ее нужно скопировать из Arduino IDE.
AVRDUDE = c:\XLoader\avrdude -Cc:\XLoader\avrdude.conf -p$(DEVICE) -c$(PROGRAMMER_TYPE) -P$(PORT) -b$(BR) -D

CC +=  -Wall -Os -DF_CPU=$(CLOCK) -mmcu=$(DEVICE)

$(BIN_DIR)/main.o: Sources/main.c $(SOURCE_H)
	$(CC) -c $(_DEPEND) -o $(_TARGET)

$(BIN_DIR)/ModbusSlave.o: ../ModBusSimple/Sources/Modbus/ModbusSlave.c $(SOURCE_H)
	$(CC) -c $(_DEPEND) -o $(_TARGET)

$(BIN_DIR)/Timer.o: $(AVR_SOURCE_LIB)/Timer/Timer.c $(SOURCE_H)
	$(CC) -c $(_DEPEND) -o $(_TARGET)

$(BIN_DIR)/Uart.o: $(AVR_SOURCE_LIB)/Uart/Uart.c $(SOURCE_H)
	$(CC) -c $(_DEPEND) -o $(_TARGET)

$(BIN_DIR)/ds18b20.o: Sources/ds18b20/ds18b20.c $(SOURCE_H)
	$(CC) -c $(_DEPEND) -o $(_TARGET)

$(BIN_DIR)/onewire.o: Sources/ds18b20/onewire.c $(SOURCE_H)
	$(CC) -c $(_DEPEND) -o $(_TARGET)

$(BIN_DIR)/romsearch.o: Sources/ds18b20/romsearch.c $(SOURCE_H)
	$(CC) -c $(_DEPEND) -o $(_TARGET)

$(BIN_DIR)/Debug.o: $(AVR_SOURCE_LIB)/Common/Debug.c $(SOURCE_H)
	$(CC) -c $(_DEPEND) -o $(_TARGET)

$(TARGET_BIN).elf: $(OBJECTS)
	$(CC) -o ${_TARGET} $^ $(SOURCE_H)

hex: $(TARGET_BIN).elf
	rm -f ${TARGET_BIN}.hex
	avr-objcopy -j .text -j .data -O ihex ${_DEPEND} ${TARGET_BIN}.hex
	avr-size --format=avr --mcu=$(DEVICE) ${_DEPEND}

clean:
	if [ -d "$BIN_DIR" ]; then rm -Rf $BIN_DIR; fi

$(BUILD_DIR):
   $(shell mkdir -p $(BIN_DIR))

all: clean $(BUILD_DIR) hex

flash:	all
	$(AVRDUDE) -U flash:w:${TARGET_BIN}.hex:i