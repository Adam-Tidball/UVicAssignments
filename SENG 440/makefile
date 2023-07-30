CC=gcc
CFLAGS=-Wall
HEADERS=HuffmanHeader.h
OBJECT=HuffmanEncoderV1.o

all: HuffmanEncoderV1

HuffmanEncoderV1: $(OBJECT)
	$(CC) -o HuffmanEncoderV1 $(OBJECT) $(CFLAGS)
	-rm -f $(OBJECT)

HuffmanEncoderV1.o: HuffmanEncoderV1.c
	$(CC) $(CFLAGS) -c HuffmanEncoderV1.c

clean:
	-rm -f $(OBJECT) HuffmanEncoderV1