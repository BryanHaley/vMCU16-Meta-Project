#include <stdlib.h>
#include <stdio.h>
#include "interpreter.h"
#include "op_codes.h"

#define FLAG_32_BIT 1
#define FLAG_16_BIT 0

int decode_form_0_instr();
int decode_form_1_instr();
int decode_form_2_instr();
int decode_form_3_instr();
int decode_form_4_instr();
int decode_form_5_instr();
int decode_form_6_instr();
int decode_form_7_instr();

op_instr* fetch()
{
    uint8_t instr_value = (uint8_t) (main_memory[*reg_pc]>>8);

    op_instr* fetched_instruction = get_instruction_by_value(instr_value);

    fetched_instr_code = (uint8_t*) calloc(WORD_BYTES, fetched_instruction->word_size);

    for (int i = 0; i < fetched_instruction->word_size; i++)
    {
        fetched_instr_code[i*WORD_BYTES]     = (uint8_t) (main_memory[*reg_pc+i]>>8);
        fetched_instr_code[(i*WORD_BYTES)+1] = (uint8_t) (main_memory[*reg_pc+i]);
    }

    return fetched_instruction;
}

int decode(op_instr* fetched_instruction)
{
    switch(fetched_instruction->format_type)
    {
        case 0: decode_form_0_instr(); break;
        case 1: decode_form_1_instr(); break;
        case 2: decode_form_2_instr(); break;
        case 3: decode_form_3_instr(); break;
        case 4: decode_form_4_instr(); break;
        case 5: decode_form_5_instr(); break;
        case 6: decode_form_6_instr(); break;
        case 7: decode_form_7_instr(); break;
    }

    return NOERR;
}

int decode_form_0_instr()
{
    format_0_instr_t* instruction = calloc(1, sizeof(format_0_instr_t));
    instruction->value = fetched_instr_code[0];
    
    next_instruction = (void*) instruction;
    next_instruction_format = 0;
   // next_instruction_size_in_bytes = fetched_instruction->word_size*WORD_BYTES;

    return NOERR;
}

int decode_form_1_instr()
{
    format_1_instr_t* instruction = calloc(1, sizeof(format_1_instr_t));
    instruction->value = fetched_instr_code[0];
    
    instruction->reg_dest = fetched_instr_code[1]>>4;
    instruction->reg_source = fetched_instr_code[1] & 0x0F;

    next_instruction = (void*) instruction;
    next_instruction_format = 1;
   // next_instruction_size_in_bytes = fetched_instruction->word_size*WORD_BYTES;

    return NOERR;
}

int decode_form_2_instr()
{
    format_2_instr_t* instruction = calloc(1, sizeof(format_2_instr_t));
    instruction->value = fetched_instr_code[0];
    
    instruction->reg_dest = fetched_instr_code[1]>>4;
    
    next_instruction = (void*) instruction;
    next_instruction_format = 2;
   // next_instruction_size_in_bytes = fetched_instruction->word_size*WORD_BYTES;

    return NOERR;
}

int decode_form_3_instr()
{
    format_3_instr_t* instruction = calloc(1, sizeof(format_3_instr_t));
    instruction->value = fetched_instr_code[0];
    
    instruction->reg_dest = fetched_instr_code[1]>>4;
    instruction->num = ((int) fetched_instr_code[2])<<8;
    instruction->num += fetched_instr_code[3];
    
    next_instruction = (void*) instruction;
    next_instruction_format = 3;
   // next_instruction_size_in_bytes = fetched_instruction->word_size*WORD_BYTES;

    return NOERR;
}

int decode_form_4_instr()
{
    format_4_instr_t* instruction = calloc(1, sizeof(format_4_instr_t));
    instruction->value = fetched_instr_code[0];
    
    instruction->reg_dest = fetched_instr_code[1]>>4;
    instruction->address  = ((int) fetched_instr_code[2])<<24;
    instruction->address |= ((int) fetched_instr_code[3])<<16;
    instruction->address |= ((int) fetched_instr_code[4])<<8;
    instruction->address |=        fetched_instr_code[5];

    instruction->msb_switch   = (fetched_instr_code[1] & 0x0F)>>2;
    instruction->address_flag = (fetched_instr_code[1] & 0x01);
    instruction->relocatable  = (fetched_instr_code[1] & 0x02)>>1;

    next_instruction = (void*) instruction;
    next_instruction_format = 4;
   // next_instruction_size_in_bytes = fetched_instruction->word_size*WORD_BYTES;

    return NOERR;
}

int decode_form_5_instr()
{
    format_5_instr_t* instruction = calloc(1, sizeof(format_5_instr_t));
    instruction->value = fetched_instr_code[0];
    
    instruction->reg_dest = fetched_instr_code[1]>>4;
    if (instruction->reg_dest < 0xF) { instruction->reg_dest_2 = instruction->reg_dest+1; }
    else { fprintf(stderr, "WARNING: Illegal instruction."); }

    //byte [2] ignored

    instruction->reg_msw = fetched_instr_code[3]>>4;
    instruction->reg_lsw = fetched_instr_code[3] & 0x0F;

    instruction->msb_switch   = (fetched_instr_code[1] & 0x0F)>>2;
    instruction->address_flag = (fetched_instr_code[1] & 0x01);
    instruction->relocatable  = (fetched_instr_code[1] & 0x02)>>1;

    next_instruction = (void*) instruction;
    next_instruction_format = 5;
   // next_instruction_size_in_bytes = fetched_instruction->word_size*WORD_BYTES;

    return NOERR;
}

int decode_form_6_instr()
{
    format_6_instr_t* instruction = calloc(1, sizeof(format_6_instr_t));
    instruction->value = fetched_instr_code[0];
    
    instruction->address_flag = (fetched_instr_code[1] & 0x01);
    instruction->relocatable  = (fetched_instr_code[1] & 0x02)>>1;

    instruction->address  = ((int) fetched_instr_code[2])<<24;
    instruction->address |= ((int) fetched_instr_code[3])<<16;
    instruction->address |= ((int) fetched_instr_code[4])<<8;
    instruction->address |=        fetched_instr_code[5];
    
    next_instruction = (void*) instruction;
    next_instruction_format = 6;
   // next_instruction_size_in_bytes = fetched_instruction->word_size*WORD_BYTES;

    return NOERR;
}

int decode_form_7_instr()
{
    format_7_instr_t* instruction = calloc(1, sizeof(format_7_instr_t));
    instruction->value = fetched_instr_code[0];
    
    instruction->reg_dest = fetched_instr_code[1]>>4;
    instruction->relocatable  = (fetched_instr_code[1] & 0x02)>>1;

    instruction->num  = ((int) fetched_instr_code[2])<<24;
    instruction->num |= ((int) fetched_instr_code[3])<<16;
    instruction->num |= ((int) fetched_instr_code[4])<<8;
    instruction->num |=        fetched_instr_code[5];
    
    next_instruction = (void*) instruction;
    next_instruction_format = 7;
   // next_instruction_size_in_bytes = fetched_instruction->word_size*WORD_BYTES;

    return NOERR;
}

/*int decode_form_0_instr()
{
    format_0_instr_t instruction* = calloc(1, sizeof(format_0_instr_t));
    instruction->value = fetched_instr_code[0];
    
    next_instruction = (void*) instruction;
    next_instruction_format = 8;
    next_instruction_size_in_bytes = fetched_instruction->word_size*WORD_BYTES;
}*/
