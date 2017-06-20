#include <stdlib.h>

#include <string.h>
#include "assembler.h"
#include "assembler_mnemonics.h"
#include "assembler_utils.h"

int lc; //this indicates the index of where an item will end up in the object file
int num_digits(int num);

/*
*   The main purpose of pass one is to find and resolve any symbols used in the source file.
*   A side effect of this is the necessity to locate where in the object file instructions
*   and data will end up, so this information is added to an intermediate source file to
*   make pass two easier.
*/
int passone()
{
    char* read_buff = NULL;
    lc = 0;

    //Cycle through all the words in the source file
    // Not to be confused with a CPU "word"
    do
    {
        read_buff = get_next_word();

        if (read_buff == NULL)
        {
            break;
        }

        //.def, .label, and .global are explicit preprocessor instructions to declare
        // a symbol
        if (strcmp(read_buff, ".def") == 0 || strcmp(read_buff, ".label") == 0 ||
            strcmp(read_buff, ".global") == 0)
        {
            char* word = get_next_word();

            //remove ':' if it's there, as it is not part of the alias
            if (word[strlen(word)-1] == ':')
            {
                word[strlen(word)-1] = '\0';
            }

            //Create a new symbol
            symbol* new_symbol = create_new_symbol(word);

            //Labels are always local
            if (strcmp(read_buff, ".label") == 0)
            {
                new_symbol->value = lc;
                new_symbol->relocatable = TRUE;
                new_symbol->external = FALSE;
            }

            //Defines are always absolute (literals)
            else if (strcmp(read_buff, ".def") == 0)
            {
                char* value_str = get_next_word();
                new_symbol->value = get_val_from_str(value_str);
                new_symbol->relocatable = FALSE;
                new_symbol->external = FALSE;
            }
            
            //Globals are symbols that the linker needs to see, as other source files
            // may reference them.
            else if (strcmp(read_buff, ".global") == 0)
            {
                new_symbol->value = lc;
                new_symbol->relocatable = TRUE;
                new_symbol->global = TRUE;
                new_symbol->external = FALSE;
            }

            //TODO: Error checking for duplicate symbols
        }

        //However, symbols may also be implicitly declared using ':'
        else if (read_buff[strlen(read_buff)-1] == ':')
        {
            char* word = calloc(1, strlen(read_buff)-1);
            memcpy(word, read_buff, strlen(read_buff)-1);
            word[strlen(word)] = '\0';

            symbol* sym = get_symbol_by_alias(word);

            //create the symbol if it doesn't already exist
            if (sym == NULL)
            {
                sym = create_new_symbol(word);
                sym->value = lc;
            }

            //otherwise, set the value to lc
            else
            {
                //unless it already has a different value, in which case it's a duplicate 
                // symbol
                if ((sym->value != 0 || lc == 0) && sym->value != lc)
                {
                    fprintf(stderr, "Error: redefinition of label %s.\n",
                            sym->alias);

                    return ERR;
                }

                sym->value = lc;
            }
        }

        //In order to keep track of where things will end up in memory, it's necessary to
        // read mnemonics and determine their machine code size
        else if (is_valid_mnemonic(read_buff))
        {
            mnemonic* instruction = get_mnemonic_by_alias(read_buff);

            char* lc_write = (char*) malloc(num_digits(lc));
            sprintf(lc_write, "%d", lc);

            //Because this information is valuable for pass two, place it into an
            // intermediate source file along with cleaned up mnemonics

            fputs("L# ", isf_file);

            for (int i = 0; i < strlen(lc_write); i++)
            {
                fputc(lc_write[i], isf_file);
            }

            fputc(' ', isf_file);

            free(lc_write);
            lc_write = NULL;

            fputs(read_buff, isf_file);
            fputc(' ', isf_file);

            int current_seek = ftell(source_file);

            int read_c_buff = 0;
            char read_c = '\0';
            char read_c_prev = '\0';

            while (read_c != '\n' && read_c != ';')
            {
                read_c_prev = read_c;
                read_c_buff = fgetc(source_file);
                if (read_c_buff == EOF) { break; }
                read_c = (char) read_c_buff;

                if (read_c != '\n' && read_c != ';' && read_c != ',')
                {
                    fputc(read_c, isf_file);
                }
            }

            fputc('\n', isf_file);

            fseek(source_file, current_seek, SEEK_SET);

            lc += instruction->word_size*WORD_BYTES;
            
            //see meta/docs/cpu_desc for formatting types
            if (instruction->format_type == 4 || instruction->format_type == 6 ||
                instruction->format_type == 7)
            {
                //get_val_from_str will generate a symbol if the str is not a number,
                // so it's an easy shortcut to check if we should generate a symbol from
                // the last operand of mnemonics with these formatting types.
                get_next_word();
                get_val_from_str(get_next_word());
            }
        }
        
        //.ascii files must be transcribed into the object file
        else if (strcmp(read_buff, ".ascii") == 0)
        {
            char* lc_write = (char*) malloc(num_digits(lc));
            sprintf(lc_write, "%d", lc);

            fputs("L# ", isf_file);

            for (int i = 0; i < strlen(lc_write); i++)
            {
                fputc(lc_write[i], isf_file);
            }

            fputc(' ', isf_file);

            fputs(".ascii ", isf_file);

            free(lc_write);
            lc_write = NULL;
            
            int read_c_buff = 0;
            char read_c = '\0';

            //Basically, just get everything between the quotes, write it to the isf,
            // and increment lc.

            //TODO: Account for escape characters, particularly \"
            while (read_c != '"')
            {
                read_c_buff = fgetc(source_file);
                if (read_c_buff == EOF) { break; }
                read_c = (char) read_c_buff;
            }
            
            do
            {
                read_c_buff = fgetc(source_file);
                if (read_c_buff == EOF) { break; }
                read_c = (char) read_c_buff;
                
                if (read_c != '"')
                {
                    fputc(read_c, isf_file);
                    lc++;
                }
            } while (read_c != '"');

            //A trailing null character may be added to keep things word (as in, a CPU
            //  16 bit word) aligned.
            if (lc%2 != 0)
            {
                 fputc('\0', isf_file);
                 lc++;
            }

            fputc('\n', isf_file);
        }

    } while (read_buff != NULL);

    //Any external symbols obviously won't be resolved in this source file, so give
    // them an assumed external address here.

    //Note: probably makes some other code redundant, fix later.

    int external_symbol_loc = 0xFFFFFFFF;

    for (int i = 0; i < num_symbols; i++)
    {
        symbol* sym = symbol_table[i];

        if (sym->external)
        {
            sym->value = external_symbol_loc;
            external_symbol_loc--;
        }
    }

    return NOERR;
}


//Quick and dirty function to get the number of non-leading-zero digits in an integer
// Might replace this with something more robust in the future
int num_digits(int num)
{
    if      (num<10) { return 1; }
    else if (num<100) { return 2; }
    else if (num<1000) { return 3; }
    else if (num<10000) { return 4; }
    else if (num<100000) { return 5; }
    else if (num<1000000) { return 6; }
    else if (num<10000000) { return 7; }
    else if (num<100000000) { return 8; }
    else if (num<1000000000) { return 9; }
    else                      { return 10; }
}
