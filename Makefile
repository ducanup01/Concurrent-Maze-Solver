CC = gcc
CFLAGS = -Iinclude -std=c11

SRC = $(wildcard src/*.c)
OUT = main

all:
	$(CC) $(SRC) $(CFLAGS) -o $(OUT)

clean:
	rm -rf main