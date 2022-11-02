.PHONY: all clean run
CC=g++
CFLAGS=-std=c++17 -O2 -Wall -Wextra
LDFLAGS=-lm
TARGET=train test
TRAIN_ITER=100

all: $(TARGET)

train: src/train.cpp
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS) -Iinc

test: src/test.cpp
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS) -Iinc

clean:
	rm -f $(TARGET)

