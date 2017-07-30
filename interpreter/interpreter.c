#include <stdio.h>
#include <stdlib.h>
#include "interpreter.h"
#include "op_codes.h"

#define DS_LOC 2 //index of the four bytes containing the address of the data section
#define TS_LOC 8 //same as above but the text (instruction) section

int main(int argc, char** argv)
{
    quit = FALSE;

    initialize_registers(); //alloc memory for registers
    initialize_valid_instructions(); //array containing some info on instructions

    memory_size = (256/WORD_BYTES)*1024; //256K
    main_memory = (uint16_t*) calloc(1, memory_size);

    //This is only for testing purposes; in the future elf files will be loaded, and the
    // file will be gotten from argv
    FILE* rom = fopen("../meta/asm_examples/helloworld.o", "r");

    //get the data section address
    fseek(rom, DS_LOC, SEEK_SET);
    uint32_t ds = 0;
    ds = ds ^ (fgetc(rom)<<24);
    ds = ds ^ (fgetc(rom)<<16);
    ds = ds ^ (fgetc(rom)<<8);
    ds = ds ^ (fgetc(rom));

    //get the text section address
    fseek(rom, TS_LOC, SEEK_SET);
    uint32_t ts = 0;
    ts = ts ^ (fgetc(rom)<<24);
    ts = ts ^ (fgetc(rom)<<16);
    ts = ts ^ (fgetc(rom)<<8);
    ts = ts ^ (fgetc(rom));

    int read_buff = 0;
    char read_msb = '\0';
    char read_lsb = '\0';
    uint16_t read_word = 0;
    int counter = 0;

    //The following is temporary for testing purposes;
    // Load the object file (minus the symbol table) into memory for execution
    fseek(rom, ds, SEEK_SET);

    while (read_buff != EOF)
    {
        read_buff = fgetc(rom);
        if (read_buff == EOF) { break; }
        read_msb = (char) read_buff;

        read_buff = fgetc(rom);
        if (read_buff == EOF) { read_buff = 0; }
        read_lsb = (char) read_buff;

        read_word = ((uint16_t) read_msb)<<8;
        read_word += read_lsb;

        main_memory[counter] = read_word;
        
        counter++;
    }

    //Set the program counter to the address of the first instruction
    *reg_pc = (ts-ds)/WORD_BYTES;

    //Add escape method later
    while(quit == FALSE)
    {
        op_instr* fetched_instruction = fetch();
        decode(fetched_instruction);
        execute();
    }

    printf("\n");
}

int initialize_registers()
{
    reg    = (uint16_t*) calloc(NUM_REG,         sizeof(uint16_t));
    reg_pc = (uint16_t*) calloc(DOUBLE_WORD_REG, sizeof(uint16_t));
    reg_ff = (uint16_t*) calloc(SINGLE_WORD_REG, sizeof(uint16_t));
    reg_cc = (uint16_t*) calloc(SINGLE_WORD_REG, sizeof(uint16_t));
    reg_sp = (uint16_t*) calloc(DOUBLE_WORD_REG, sizeof(uint16_t));
    reg_bp = (uint16_t*) calloc(DOUBLE_WORD_REG, sizeof(uint16_t));
    reg_fp = (uint16_t*) calloc(DOUBLE_WORD_REG, sizeof(uint16_t));
    reg_pf = (uint16_t*) calloc(SINGLE_WORD_REG, sizeof(uint16_t));

    return NOERR;
}
