#include <stdlib.h>
#include <string.h>
#include "assembler.h"
#include "assembler_mnemonics.h"
#include "assembler_utils.h"

#define WORD_BYTES 2

#define DS_IDEN_LOC 2 //Start of the data section in object file
#define TS_IDEN_LOC 8 //Start of the text (instruction) section

//once passtwo reads the first mnemonic it'll record the address at TS_IDEN_LOC,
// so first_mnemonic prevents it from doing this every time it reads a mnemonic.
int first_mnemonic = TRUE;

//see meta/docs/cpu_desc for info on formatting types
int type_0_mnemonic(mnemonic* instruction);
int type_1_mnemonic(mnemonic* instruction);
int type_2_mnemonic(mnemonic* instruction);
int type_3_mnemonic(mnemonic* instruction);
int type_4_mnemonic(mnemonic* instruction);
int type_5_mnemonic(mnemonic* instruction);
int type_6_mnemonic(mnemonic* instruction);
int type_7_mnemonic(mnemonic* instruction);
int type_8_mnemonic(mnemonic* instruction);

int passtwo()
{
    //pass fseek 0 just in case
    start_from_beginning();

    char* read_buff = NULL;

    //The 0000's are place holders for a 32 bit address
    fputs("DS0000", object_file);
    fputs("TS0000", object_file);

    //Create a table of globals from symbols marked as global in the symbol table
    uint32_t num_globals = 0;

    symbol** globals_table = calloc(1, sizeof(symbol*));

    for (int i = 0; i < num_symbols; i++)
    {
        symbol* sym = symbol_table[i];
        if (sym->global)
        {
            num_globals++;
            globals_table = (symbol**) realloc(globals_table, num_globals*sizeof(symbol*));
            globals_table[num_globals-1] = sym;
        }
    }

    //Separate the 32 bit number num_globals into 8 bit bytes we can
    // easily write to the object file.
    char* num_globals_str = (char*) calloc(sizeof(char), sizeof(uint32_t));
    num_globals_str[0] = (char) num_globals>>24;
    num_globals_str[1] = (char) num_globals>>16;
    num_globals_str[2] = (char) num_globals>>8;
    num_globals_str[3] = (char) num_globals;

    //Record the number of global symbols in the object file prefixed with "S#"
    fputs("S#", object_file);
    for (int i = 0; i < sizeof(uint32_t); i++) { fputc(num_globals_str[i], object_file); }
    fputc('\n', object_file);

    //Record each global symbol in the object file
    //Format: S E ALIAS #### (no actual spaces)
    // where # is a 32 bit address, ALIAS is an ascii string up to 255 characters long,
    // and E indicates if the symbol is external or not.
    for (int i = 0; i < num_globals; i++)
    {
        fputc('S', object_file);
        fputc((char) globals_table[i]->external, object_file);
        
        //If the alias is greater than 255 characters for some reason, cut it at 255

        char* alias = globals_table[i]->alias;
        int length = strlen(alias);
        if (length > 255)
        {
            length = 255;
            alias = (char*) malloc(length);
            memcpy(alias, globals_table[i]->alias, length);
        }
        char length_c = (char) length;

        //Write the length then alias to the object file

        fputc(length_c, object_file);

        fputs(alias, object_file);

        //Convert the 32 bit address to an array of one byte characters to write

        uint32_t address = globals_table[i]->value;
        char* address_str = calloc(sizeof(char), sizeof(uint32_t));

        address_str[0] = (char) address>>24;
        address_str[1] = (char) address>>16;
        address_str[2] = (char) address>>8;
        address_str[3] = (char) address;

        //write the address

        for (int j = 0; j < sizeof(uint32_t); j++)
        {
            fputc(address_str[j], object_file);
        }
    }

    //The global symbols table is only for the linker; it will not be copied into memory.
    // Therefore, memory references made by instructions should not include the offset made
    // by the assembler object header. Record the offset here.

    int offset = ftell(object_file);

    //Record where the data section starts. For now I'm assuming the data section comes
    // first, but in the near future I'll accomodate either the text or data sections
    // coming first.

    //TODO: Differentiate between rodata and data sections, and accomodate that here.

    fseek(object_file, DS_IDEN_LOC, SEEK_SET);
    fputc((char) (offset>>24), object_file);
    fputc((char) (offset>>16), object_file);
    fputc((char) (offset>>8), object_file);
    fputc((char) (offset), object_file);
    fseek(object_file, offset, SEEK_SET);

    do
    {
        read_buff = get_next_word();

        if (read_buff == NULL)
        {
            break;
        }

        if (strcmp(read_buff, "L#") == 0)
        {
            char* index_str = get_next_word();
            int index = (int) strtol(index_str, (char**) NULL, 10);

            fseek(object_file, index+offset, SEEK_SET);
        }

        // If we read a mnemonic, transcribe that into a machine code instruction
        if (is_valid_mnemonic(read_buff))
        {
            if (first_mnemonic)
            {
                first_mnemonic = FALSE;

                int current_loc = ftell(object_file);

                fseek(object_file, TS_IDEN_LOC, SEEK_SET);
                fputc((char) (current_loc>>24), object_file);
                fputc((char) (current_loc>>16), object_file);
                fputc((char) (current_loc>>8), object_file);
                fputc((char) (current_loc), object_file);
                fseek(object_file, current_loc, SEEK_SET);
            }
            
            mnemonic* instruction = get_mnemonic_by_alias(read_buff);

            int err = NOERR;

            //Every instruction has a "format" it adheres to, to make things
            // easier. See meta/docs/cpu_desc

            switch (instruction->format_type)
            {
                case 0: err = type_0_mnemonic(instruction); break;
                case 1: err = type_1_mnemonic(instruction); break;
                case 2: err = type_2_mnemonic(instruction); break;
                case 3: err = type_3_mnemonic(instruction); break;
                case 4: err = type_4_mnemonic(instruction); break;
                case 5: err = type_5_mnemonic(instruction); break;
                case 6: err = type_6_mnemonic(instruction); break;
                case 7: err = type_7_mnemonic(instruction); break;
                case 8: err = type_8_mnemonic(instruction); break;
                default:
                    fprintf(stderr, "Mnemonic %s of unknown type read.\n", read_buff);
                    break;
            }

            if (err < NOERR)
            {
                fprintf(stderr, "Error translating instructions.\n");
                return ERR;
            }
        }

        //Transcribe ascii strings into the object file
        if (strcmp(read_buff, ".ascii") == 0)
        {
            int read_c_buff = 0;
            char read_c = '\0';
            char read_c_prev = '\0';
            
            //keep reading and writing until we get to a newline character
            //TODO: accomodate for newlines and other special characters in the string
            while (read_c != '\n')
            {
                read_c_prev = read_c;
                read_c_buff = fgetc(source_file);
                if (read_c_buff == EOF) { break; }
                read_c = (char) read_c_buff;

                if (read_c != '\n')
                {
                    fputc(read_c, object_file);
                }
            }
        }

    } while (read_buff != NULL);

    return NOERR;
}

// SEE meta/docs/cpu_desc FOR INFO ON FORMATTING TYPES
// TODO: Much more error checking

int type_0_mnemonic(mnemonic* instruction)
{
    //get the length (in 16 bit words) of the instruction
    int length = instruction->word_size*WORD_BYTES;
    char* machine_code = (char*) calloc(instruction->word_size, WORD_BYTES);

    //get the indentifying value of the instruction (see meta/docs/cpu_desc)
    machine_code[0] = instruction->value;

    //Type 0 doesn't have any operands, so just write what we have
    for (int i = 0; i < length; i++)
    {
        fputc(machine_code[i], object_file);
    }

    return NOERR;
}

int type_1_mnemonic(mnemonic* instruction)
{
    int length = instruction->word_size*WORD_BYTES;
    char* machine_code = (char*) calloc(instruction->word_size, WORD_BYTES);

    machine_code[0] = instruction->value;

    //Get the first register operand and convert it to machine code
    char* buff = get_next_word();
    machine_code[1] = (char) (get_val_from_hex_char(buff[1])<<4);

    free(buff); buff = NULL;

    //Get the second register operand and OR it into the right place
    buff = get_next_word();
    machine_code[1] = machine_code[1] ^ get_val_from_hex_char(buff[1]);
    
    free(buff); buff = NULL;

    //Write the machine code
    for (int i = 0; i < length; i++)
    {
        fputc(machine_code[i], object_file);
    }

    return NOERR;
}

int type_2_mnemonic(mnemonic* instruction)
{
    int length = instruction->word_size*WORD_BYTES;
    char* machine_code = (char*) calloc(instruction->word_size, WORD_BYTES);

    machine_code[0] = instruction->value;
    
    //Get the first register operand
    char* buff = get_next_word();
    machine_code[1] = (char) (get_val_from_hex_char(buff[1])<<4);

    free(buff); buff = NULL;

    //Type 2 mnemonics have no other operands
    for (int i = 0; i < length; i++)
    {
        fputc(machine_code[i], object_file);
    }

    return NOERR;
}

int type_3_mnemonic(mnemonic* instruction)
{
    int length = instruction->word_size*WORD_BYTES;
    char* machine_code = (char*) calloc(instruction->word_size, WORD_BYTES);

    machine_code[0] = instruction->value;

    //Get the register operand
    char* buff = get_next_word();
    machine_code[1] = (char) (get_val_from_hex_char(buff[1])<<4);

    free(buff); buff = NULL;

    buff = get_next_word();

    //Get the 16 bit literal number operand
    uint16_t literal = (uint16_t) get_val_from_str(buff);

    free(buff); buff = NULL;

    //convert it to machine code
    machine_code[2] = (char) (literal>>8);
    machine_code[3] = (char) literal;

    for (int i = 0; i < length; i++)
    {
        fputc(machine_code[i], object_file);
    }

    return NOERR;
}

int type_4_mnemonic(mnemonic* instruction)
{
    int length = instruction->word_size*WORD_BYTES;
    char* machine_code = (char*) calloc(instruction->word_size, WORD_BYTES);

    machine_code[0] = instruction->value;

    //Get the register operand
    char* buff = get_next_word();
    machine_code[1] = (char) (get_val_from_hex_char(buff[1])<<4);

    //Type 4 mnemonics might specify whether to use the most significant (high) byte,
    // or the least significant (low) byte, or both bytes of either a register, or a
    // word at a certain address. In the mnemonic this is indicated by h, l, or the lack
    // thereof, so read for that and OR the proper value in place.
    if (strlen(buff) < 3)
    { machine_code[1] = machine_code[1] ^ (0x2<<2); }

    else if (buff[2] == 'h' || buff[2] == 'H')
    { machine_code[1] = machine_code[1] ^ (0x1<<2); }

    //get the address operand
    char* adr_buff = get_next_word();
    
    //Determine if the operand is an absolute or relocatable address
    if (!is_str_number(adr_buff))
    {
        symbol* sym = get_symbol_by_alias(adr_buff);
        
        if (sym->relocatable)
        {
            //OR the flag accordingly
            machine_code[1] = machine_code[1] ^ (0x1<<1);
        }
    }
    
    //convert it to a number
    uint32_t adr = get_val_from_str(adr_buff);
    
    free(adr_buff); adr_buff = NULL;
    free(buff); buff = NULL;

    //If the address is 32 bit, i.e. the first 16 bits do not equal zero, set the flag
    // that indicates this to the interpreter as such.
    if ((adr & 0xFFFF0000) != 0)
    {
        machine_code[1] = machine_code[1] ^ 0x1;
        //Convert the first 16 bits of the address to machine code
        machine_code[2] = (char) (adr>>24);
        machine_code[3] = (char) (adr>>16);
    }

    //Convert the last sixtenn bits to machine code
    machine_code[4] = (char) (adr>>8);
    machine_code[5] = (char) (adr);

    //Write the complete machine code
    for (int i = 0; i < length; i++)
    {
        fputc(machine_code[i], object_file);
    }

    return NOERR;
}

int type_5_mnemonic(mnemonic* instruction)
{
    int length = instruction->word_size*WORD_BYTES;
    char* machine_code = (char*) calloc(instruction->word_size, WORD_BYTES);

    machine_code[0] = instruction->value;

    //Get the first register operand
    char* buff = get_next_word();
    machine_code[1] = (char) (get_val_from_hex_char(buff[1])<<4);

    //Like type 4 above, type 5 mnemonics might specify high or low bytes
    if (strlen(buff) < 3)
    { machine_code[1] = machine_code[1] ^ 0x2<<2; }
    
    else if (buff[2] == 'H' || buff[2] == 'h')
    { machine_code[1] = machine_code[1] ^ 0x1<<2; }

    free(buff); buff = NULL;

    //Type 5 instructions have a byte set to zero here for word alignment

    //Get the second and third register operands
    char* regw_buff = get_next_word();
    char* regx_buff = get_next_word();

    machine_code[3] = (char) (get_val_from_hex_char(regw_buff[1])<<4);
    machine_code[3] = machine_code[3] ^
                        (char) (get_val_from_hex_char(regx_buff[1]));

    free(regw_buff); regw_buff = NULL;
    free(regx_buff); regx_buff = NULL;

    for (int i = 0; i < length; i++)
    {
        fputc(machine_code[i], object_file);
    }

    return NOERR;
}

int type_6_mnemonic(mnemonic* instruction)
{
    int length = instruction->word_size*WORD_BYTES;
    char* machine_code = (char*) calloc(instruction->word_size, WORD_BYTES);

    machine_code[0] = instruction->value;

    //Type 6 mnemonics have no register operand; they're usually jump instructions

    //Get the address operand
    char* adr_buff = get_next_word();
    
    //Determine if the address is absolute or relocatable
    if (!is_str_number(adr_buff))
    {
        symbol* sym = get_symbol_by_alias(adr_buff);
        
        if (sym->relocatable)
        {
            machine_code[1] = machine_code[1] ^ (0x1<<1);
        }
    }
    
    uint32_t adr = get_val_from_str(adr_buff);
    
    //Check if the address is 16 or 32 bits, like type 4
    if ((adr & 0xFFFF0000) != 0)
    {
        machine_code[1] = 0x01;
        machine_code[2] = (char) (adr>>24);
        machine_code[3] = (char) (adr>>16);
    }

    free(adr_buff); adr_buff = NULL;

    machine_code[4] = (char) (adr>>8);
    machine_code[5] = (char) (adr);

    for (int i = 0; i < length; i++)
    {
        fputc(machine_code[i], object_file);
    }

    return NOERR;
}

int type_7_mnemonic(mnemonic* instruction)
{
    int length = instruction->word_size*WORD_BYTES;
    char* machine_code = (char*) calloc(instruction->word_size, WORD_BYTES);

    machine_code[0] = instruction->value;
    
    //get the register operand
    char* buff = get_next_word();
    machine_code[1] = (char) (get_val_from_hex_char(buff[1])<<4);

    free(buff); buff = NULL;

    char* adr_buff = get_next_word();
    
    //Determine if the address is absolute or relocatable
    if (!is_str_number(adr_buff))
    {
        symbol* sym = get_symbol_by_alias(adr_buff);
        
        if (sym != NULL) if (sym->relocatable)
        {
            machine_code[1] = machine_code[1] ^ (0x1<<1);
        }
    }
    
    uint32_t adr = get_val_from_str(adr_buff);
    
    //Determine if the address is 16 or 32 bits
    if ((adr & 0xFFFF0000) != 0)
    {
        machine_code[1] = 0x01;
        machine_code[2] = (char) (adr>>24);
        machine_code[3] = (char) (adr>>16);
    }

    free(adr_buff); adr_buff = NULL;

    machine_code[4] = (char) (adr>>8);
    machine_code[5] = (char) (adr);

    for (int i = 0; i < length; i++)
    {
        fputc(machine_code[i], object_file);
    }

    return NOERR;
}

// NOTE: lots of reused code above, consider putting some of it into functions

// Type 8 mnemonics are those that have formats unique to them, or perhaps their instruction
//  isn't immediately obvious from their mnemonic. Will these later; unnecessary right now. 
int type_8_mnemonic(mnemonic* instruction)
{
    return NOERR;
}

