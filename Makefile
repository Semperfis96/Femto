CC        = gcc
CFLAGS    = -Wall -Wextra -g
CLIBS     =
BUILD_DIR = ./build
SRC_DIR   = ./src


default: all


$(BUILD_DIR)/femto.o: $(SRC_DIR)/femto.c
	$(CC) -c -o $@ $< $(CFLAGS) $(CLIBS)

$(BUILD_DIR)/asm.o: $(SRC_DIR)/asm.c
	$(CC) -c -o $@ $< $(CFLAGS) $(CLIBS)

$(BUILD_DIR)/dism.o: $(SRC_DIR)/dism.c
	$(CC) -c -o $@ $< $(CFLAGS) $(CLIBS)

$(BUILD_DIR)/test.o: $(SRC_DIR)/test/test.c
	$(CC) -c -o $@ $< $(CFLAGS) $(CLIBS)

test: $(BUILD_DIR)/test.o
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/test $< $(CLIBS)

femto: $(BUILD_DIR)/femto.o
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/femto $< $(CLIBS)

asm: $(BUILD_DIR)/asm.o
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/asm $< $(CLIBS)

dism: $(BUILD_DIR)/dism.o
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/dism $< $(CLIBS)

all: femto asm dism test


.PHONY: clean

clean:
	rm -f ./build/*.o ./build/femto ./build/asm ./build/dism ./build/test