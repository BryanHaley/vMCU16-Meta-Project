#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <stdint.h>
#include <stdio.h>
#include "assembler_mnemonics.h"

#define NOERR 0
#define ERR -1
#define TRUE 1
#define FALSE 0
#define WORD_BYTES 2 // the vCPU is 16 bit, so one word is 2 bytes

//main function for assembling files; accepts file paths as strings
int assemble(char* source_file_str, char* object_file_str);

FILE* source_file; // this is the human-readable source file provided from the command line
FILE* isf_file; // this is a modified source file generated to make the second pass easier
FILE* object_file; // this is the final assembled object file

uint32_t external_offset; // if a symbol cannot be resolved, it is assumed to be external,
                          // given the address 0xFFFFFFFF minus the number of external
                          // references.

// Future note: a program using a large amount of memory may run into trouble with local
// symbols using the same address as an external symbol. Add error checking for this.

/* A symbol can be one of three things:
    * A reference to a memory location known in a different source file (external)
    * A reference to a memory location known in this source file (local)
    * A user-created mnemonic to be replaced by some arbitrary value, similar
        to #define in C.
*/
typedef struct
{
    char* alias;
    uint32_t value;
    int relocatable; // this tells the linker if it's an address or an arbitrary value
    int global; // tells the linker this symbol can be used by other source files
    int external; // tells the linker this symbol is expected to be found elsewhere
} symbol;

symbol** symbol_table;
int num_symbols;

symbol* create_new_symbol(char* alias);
symbol* get_symbol_by_alias(char* alias);

//pass one reads the source file, calculates the location in the file where each instruction
//goes, adds any symbols and their values to the symbol table, then generates an
//intermediate source file to make pass two easier
int passone();

//Pass two reads the intermediate source file, writes necessary symbols to a small header
//at the top of the object file for the linker, writes the data section to the object file,
//the translates the mnemonics into machine code and generates the object file.
int passtwo();

#endif
