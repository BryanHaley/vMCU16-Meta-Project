# vim:noet
CC=gcc
CFLAGS=-std=c99 -Wall -g -I.

objs:
	$(CC) $(CFLAGS) -c interpreter.c
all: objs
	$(CC) -o interpreter interpreter.o
clean:
	-rm interpreter.o
	-rm -r ._*
