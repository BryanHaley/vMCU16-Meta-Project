#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "assembler.h"
#include "assembler_utils.h"
#include "assembler_mnemonics.h"
#include "assembler_options.h"

//Initialize, then run both passes (see .h file)
int main (int argc, char* argv[])
{
    initialize_valid_mnemonics();
    initialize_default_options();

    for (int i = 1; i < argc; i++)
    {
        char* ext = strrchr(argv[i], '.');
        
        if (argv[i][0] == '-')
        {
            parse_option(argv[i]);
        }

        else if (ext && strcmp(ext, ".asm") == 0)
        {
            char* source_file_str = argv[i];
            char* object_file_str = calloc(1, strlen(source_file_str)-2);
            memcpy(object_file_str, source_file_str, strlen(source_file_str)-4);
            strncat(object_file_str, ".o", strlen(source_file_str)-2);
            assemble(source_file_str, object_file_str);
        }
    }

    return NOERR;
}

int assemble(char* source_file_str, char* object_file_str)
{
    external_offset = 0xFFFFFFFF;

    char* isf_file_str = calloc(1, strlen(source_file_str));
    memcpy(isf_file_str, source_file_str, strlen(source_file_str)-4);
    strncat(isf_file_str, ".isf", strlen(source_file_str));
    
    open_source_file(source_file_str);
    open_isf_file(isf_file_str);    

    if (passone() < NOERR)
    {
        fprintf(stderr, "Error in symbol table generation. Assembler aborted.\n");
        return ERR;
    }

    close_source_file();
    close_isf_file();

    open_source_file(isf_file_str);
    open_object_file(object_file_str);

    if (passtwo() < NOERR)
    {
        fprintf(stderr, "Error in translation of mnemonics. Assembler aborted.\n");
        return ERR;
    }
    
    close_object_file();
    close_source_file();

    if (!(*assembler_keep_isf))
    { remove(isf_file_str); }

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
