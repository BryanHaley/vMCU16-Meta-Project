# vim:noet
CC=gcc
CFLAGS=-std=c99 -Wall -g -I.

objs:
	$(CC) $(CFLAGS) -c interpreter.c op_codes.c execute.c fetch_decode.c
all: objs
	$(CC) -o interpreter interpreter.o op_codes.o execute.o fetch_decode.o
clean:
	-rm interpreter.o op_codes.o execute.o fetch_decode.o
	-rm -r ._*
