#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "assembler.h"
#include "assembler_utils.h"

int in_data_section;

// Might be superfluous to wrap fopen; consider removing later.
int open_source_file(char* filename)
{
    source_file = fopen(filename, "r");

    if (source_file == NULL)
    {
        fprintf(stderr, "Error opening file %s\n", filename);
        return ERR;
    }

    in_data_section = FALSE;

    return NOERR;
}

//get_next_word returns only text that may be relevant to the assembler; i.e. ignoring
//  whitespace and comments.
char* get_next_word()
{
    int done = FALSE;
    int found_word = FALSE;
    char* word;
    int word_size = 1;

    //to use realloc I must first malloc/calloc, and it's safe to assume whatever this
    // returns will at least be one character long
    word = (char*) calloc(1, word_size);
    *word = '\0';

    //loop through the source file until we get a complete word
    //not to be confused with a CPU "word"
    while (!done)
    {
        read_buff = fgetc(source_file);
        
        if (read_buff < 0) // this usually means end of file, so it isn't necessarily an
                           // error
        {
            return NULL;
        }

        read_char = (char) read_buff;

        // If the rest of the line is a comment, skip to the next line
        if (read_char == ';' && !found_word)
        {
            while (read_char != '\n')
            {
                read_buff = fgetc(source_file);
                if (read_buff < 0) { return NULL; }
                read_char = (char) read_buff;
            }
        }

        if (strcmp(word, ".rodata") == 0 || strcmp(word, ".data") == 0)
        {
            //instr_line = -1;
            //This is probably unnecessary now. Consider deletion later.
            in_data_section = TRUE;
        }

        else if (strcmp(word, ".text") == 0)
        {
            in_data_section = FALSE;
        }

        // Originally get_next_word would handle where in memory the instructions would
        // end up, but it made more sense to allow pass one to handle this instead.
        /*if (read_char == '\n' && is_valid_mnemonic(word) && !in_data_section)
        {
            mnemonic* instruction = get_mnemonic_by_alias(word);
            instr_line += instruction->word_size*WORD_BYTES;
        }*/

        /*else if (read_char == '\n' && !(strcmp(word, ".def") == 0) 
                 && !(strcmp(word, ".label") == 0) &&
                 in_data_section && word[0] != '\0')
        {
            instr_line++;
        }*/

        //If we read something other than a space/semi-colon/formatting character, add it to
        // the buffer.
        if (read_char != ' ' && read_char != '\t' && read_char != ','
            && read_char != '\n')
        {
            if (!found_word) found_word = TRUE;
            *(word+word_size-1) = read_char;
            word_size++;
            word = (char*) realloc(word, word_size);
        }

        //Otherwise, if we have stuff in the buffer and we read a space, newline, etc,
        // return it.
        else if ((read_char == ' '  || read_char == '\t' ||
                  read_char == '\n' || read_char == ';') &&
                  found_word)
        {
            *(word+word_size) = '\0';
            return word;
        }
    }

    // This memory needs to be freed from whatever calls this function.
    // Note to self: go through and clean up memory leaks later.
    return word;
}

// Reads until it finds a new line. Ended up not being useful, might delete.
int skip_to_next_line()
{
    do
    {
        read_buff = fgetc(source_file);

        if (read_buff < 0)
        {
            return ERR;
        }

        read_char = (char) read_buff;
    } while (read_char != '\n');

    return NOERR;
}

//Gets everything between the current position and the next new line. Ended up not being
// useful; might delete.
char* get_next_line()
{
    int done = FALSE;
    char* line;
    int line_size = 1;

    line = (char*) calloc(1, line_size);

    while (!done)
    {
        read_buff = fgetc(source_file);
        
        if (read_buff < 0) // this usually means end of file, so it isn't necessarily an
                           // error
        {
            return NULL;
        }

        read_char = (char) read_buff;

        if ((read_char == ' ' || read_char == '\t') && line_size > 1)
        {
            if (!(*(line+line_size-2) == ' '))
            {
                *(line+line_size-1) = ' ';
                line_size++;
                line = (char*) realloc(line, line_size);
            }
        }

        else if (read_char != '\n' && read_char != ',')
        {
            *(line+line_size-1) = read_char;
            line_size++;
            line = (char*) realloc(line, line_size);
        }

        else if (read_char == '\n')
        {
            *(line+line_size) = '\0';
            return line;
        }
    }

    return NULL;
}

// It's easy enough to just call fseek, I should delete this.
int start_from_beginning()
{
    return fseek(source_file, 0, SEEK_SET);
}

int close_source_file()
{
    int ret = fclose(source_file);
    source_file = NULL;
    return ret;
}

int open_isf_file(char* filename)
{
    isf_file = fopen(filename, "w+");

    if (isf_file == NULL)
    {
        fprintf(stderr, "Error opening file %s\n", filename);
        return ERR;
    }

    return NOERR;
}

int close_isf_file()
{
    int ret = fclose(isf_file);
    isf_file = NULL;
    return ret;
}

int open_object_file(char* filename)
{
    object_file = fopen(filename, "wb+");

    if (object_file == NULL)
    {
        fprintf(stderr, "Error opening file %s\n", filename);
        return ERR;
    }

    return NOERR;
}

//Ended up just calling fputc instead of using fwrite.
int write_words(uint16_t* words, int length)
{
    return fwrite(words, WORD_BYTES, length, object_file);
}

int close_object_file()
{
    int ret = fclose(object_file);
    object_file = NULL;
    return ret;
}

//Simply returns if the string contains only numerical characters or not
int is_str_number(char* str)
{
    // Note on usage: this does NOT account for hex, as hex is always
    // assumed to be prefixed with "0x"

    for (int i = 0; i < strlen(str); i++)
    {
        if (str[i] < '0' || str[i] > '9')
        {
            return FALSE;
        }
    }

    return TRUE;
}

// Either converts the string into a number, or if the string is not a number,
// it must be a symbol. If the symbol exists, the value for the symbol is returned.
// If the symbol does not exist, it's assumed to be external, so a new symbol with
// an assumed external address is created, and its value returned.
uint32_t get_val_from_str(char* str)
{
    if (strlen(str) > 2) if (str[0] == '0' && str[1] == 'x')
    { return get_val_from_hex_str(str); }

    if (!is_str_number(str))
    {
        symbol* sym = get_symbol_by_alias(str);

        if (sym == NULL)
        {
            sym = create_new_symbol(str);
            sym->value = external_offset;
            sym->global = TRUE;
            sym->external = TRUE;

            external_offset--;
        }

        return sym->value;
    }

    return (uint32_t) strtol(str, (char**) NULL, 10);
}

//Converts a hex string into an integer
uint32_t get_val_from_hex_str(char* str)
{
    char* hex_str = NULL;
    uint32_t result = 0;

    if (str[0] == '0' && str[1] == 'x')
    {
        hex_str = (char*) calloc(1, strlen(str)-2+1);
        memcpy(hex_str, str+2, strlen(str)-2+1);
    }

    for (int i = strlen(hex_str)-1; i >= 0; i--)
    {
        int num = get_val_from_hex_char(hex_str[i]);        

        result += num<<(strlen(hex_str)-i-1)*4;
    }

    free(hex_str);
    hex_str = NULL;

    return result;
}

//Converts a single hex character represented by ASCII to its numerical representation
uint32_t get_val_from_hex_char(char ch)
{
    uint32_t num = 0;

    switch (ch)
    {
        case 'A': case 'a':
            num = 10; break;
        case 'B': case 'b':
            num = 11; break;
        case 'C': case 'c':
            num = 12; break;
        case 'D': case 'd':
            num = 13; break;
        case 'E': case 'e':
            num = 14; break;
        case 'F': case 'f':
            num = 15; break;
        default:
            num = ch-'0'; break; //If it's zero through nine, the ascii value for the
                                 //character '0' can simply be subtracted from it
        }

    return num;
}
