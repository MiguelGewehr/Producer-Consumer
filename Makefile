CC = gcc
CFLAGS = -Wall -g -pthread

all:
	mkdir -p build
	$(CC) $(CFLAGS) src/*.c -o build/main

run: all
	./build/main

clean:
	rm -rf build

.PHONY: all run clean