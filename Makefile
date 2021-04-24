CC        = gcc
CFLAGS    = -Wall -Wextra -g
CLIBS     =
BUILD_DIR = ./build
SRC_DIR   = ./src
OBJS      = $(BUILD_DIR)/main.o $(BUILD_DIR)/femto.o $(BUILD_DIR)/cpu.o


default: all


# Emulator building
$(BUILD_DIR)/main.o: $(SRC_DIR)/main.c
	$(CC) -c -o $@ $< $(CFLAGS) $(CLIBS)

$(BUILD_DIR)/femto.o: $(SRC_DIR)/femto.c
	$(CC) -c -o $@ $< $(CFLAGS) $(CLIBS)

$(BUILD_DIR)/cpu.o: $(SRC_DIR)/cpu/cpu.c
	$(CC) -c -o $@ $< $(CFLAGS) $(CLIBS)


# Tools bulding
$(BUILD_DIR)/asm.o: $(SRC_DIR)/utils/asm.c
	$(CC) -c -o $@ $< $(CFLAGS) $(CLIBS)

$(BUILD_DIR)/dism.o: $(SRC_DIR)/utils/dism.c
	$(CC) -c -o $@ $< $(CFLAGS) $(CLIBS)


# Test building
$(BUILD_DIR)/test.o: $(SRC_DIR)/test/test.c
	$(CC) -c -o $@ $< $(CFLAGS) $(CLIBS)



main: $(OBJS)
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/femto $(OBJS) $(CLIBS)

test: $(BUILD_DIR)/test.o $(BUILD_DIR)/cpu.o
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/test $(BUILD_DIR)/test.o $(BUILD_DIR)/cpu.o $(CLIBS)

asm: $(BUILD_DIR)/asm.o
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/asm $< $(CLIBS)

dism: $(BUILD_DIR)/dism.o
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/dism $< $(CLIBS)

all: main asm dism test


.PHONY: clean

clean:
	rm -f ./build/*.o ./build/femto ./build/asm ./build/dism ./build/test