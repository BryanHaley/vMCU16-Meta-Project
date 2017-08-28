# vim:noet
CC=gcc
CFLAGS=-std=c99 -Wall -g -I.

objs:
	$(CC) $(CFLAGS) -c assembler.c assembler_utils.c assembler_mnemonics.c passone.c passtwo.c assembler_options.c
all: objs
	$(CC) -o assembler assembler.o assembler_utils.o assembler_mnemonics.o passone.o passtwo.o assembler_options.o
clean:
	-rm assembler assembler.o assembler_utils.o assembler_mnemonics.o passone.o passtwo.o assembler_options.o
	-rm -r ._*
