#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "assembler.h"
#include "assembler_utils.h"
#include "assembler_mnemonics.h"

int initialize()
{
    initialize_valid_mnemonics();

    external_offset = 0xFFFFFFFF;

    //This is temporary; only for testing purposes
    open_source_file("../asm_examples/helloworld.asm");
    open_isf_file("../asm_examples/helloworld.isf");    

    //In the future this method will interpret command line options from argv
}

//Initialize, then run both passes (see .h file)
int main (int argc, char* argv[])
{
    initialize();

    if (passone() < NOERR)
    {
        fprintf(stderr, "Error in symbol table generation. Assembler aborted.\n");
        return ERR;
    }

    close_source_file();
    close_isf_file();

    //temporary
    open_source_file("../asm_examples/helloworld.isf");
    open_object_file("../asm_examples/helloworld.o");

    if (passtwo() < NOERR)
    {
        fprintf(stderr, "Error in translation of mnemonics. Assembler aborted.\n");
        return ERR;
    }
    
    close_object_file();
    close_source_file();

    return NOERR;
}

// Dynamically allocate more memory for a new symbol to be added to the symbol table
symbol* create_new_symbol(char* alias)
{
    symbol* new_symbol = calloc(1, sizeof(symbol));
    
    char* temp = calloc(1, strlen(alias)+1);
    memcpy(temp, alias, strlen(alias)+1);
    
    //some defaults
    new_symbol->alias = temp;
    new_symbol->value = 0;
    new_symbol->relocatable = FALSE;
    new_symbol->global = FALSE;
    new_symbol->external = TRUE; //Assume it's external until the reference is found

    if (symbol_table == NULL)
    {
        symbol_table = (symbol**) calloc(1, sizeof(symbol*));
        num_symbols = 0;
    }

    num_symbols++;
    symbol_table = (symbol**) realloc(symbol_table, num_symbols*sizeof(symbol*));

    //add the pointer to the table
    symbol_table[num_symbols-1] = new_symbol;

    return new_symbol;
}

symbol* get_symbol_by_alias(char* alias)
{
    //quick and dirty linear search; optimize later
    for (int i = 0; i < num_symbols; i++)
    {
        if (strcmp(symbol_table[i]->alias, alias) == 0)
        {
            return symbol_table[i];
        }
    }

    return NULL;
}
