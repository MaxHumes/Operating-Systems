CC = gcc
CFLAGS = -g -Wall

all: memgrind

umalloc: umalloc.c
	$(CC) $(CFLAGS) -o mm umalloc.c

memgrind: umalloc.c memgrind.c
	$(CC) $(CFLAGS) -c umalloc.c
	$(CC) $(CFLAGS) -o memgrind memgrind.c umalloc.o
clean:
	rm -f umalloc.o memgrind
