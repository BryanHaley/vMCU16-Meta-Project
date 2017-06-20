# vim:noet
CC=gcc
CFLAGS=-std=c99 -Wall -g -I.

objs:
	$(CC) $(CFLAGS) -c linker.c
all: objs
	$(CC) -o linker linker.o
clean:
	-rm linker.o
	-rm -r ._*
