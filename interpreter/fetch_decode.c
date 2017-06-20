#include "interpreter.h"

int decode_form_0_instr();
int decode_form_1_instr();
int decode_form_2_instr();
int decode_form_3_instr();
int decode_form_4_instr();
int decode_form_5_instr();
int decode_form_6_instr();
int decode_form_7_instr();
int decode_form_8_instr();

int fetch()
{
    uint8_t instr_value = main_memory[*reg_pc];

    fetched_instruction = get_instruction_by_value(instr_value);

    fetched_instr_code = (uint8_t*) calloc(WORD_BYTES, instruction->word_size);

    fetched_instr_code[0] = instr_value;

    for (int i = 1; i < fetched_instruction->word_size*WORD_BYTES;i++)
    { fetched_instr_code[i] = main_memory[*reg_pc+i]; }

    return NOERR;
}

int decode()
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
        case 8: decode_form_8_instr(); break;
   }
}

int decode_form_0_instr()
{
    format_0_instr_t instruction* = malloc(sizeof(format_0_instr_t));
    instruction->value = fetched_instr_code[0];
    
    next_instruction = (void*) instruction;
    next_instruction_format = 0;
    next_instruction_size_in_bytes = fetched_instruction->word_size*WORD_BYTES;
}

int decode_form_1_instr()
{
    format_1_instr_t instruction* = malloc(sizeof(format_1_instr_t));
    instruction->value = fetched_instr_code[0];
    
    instruction->reg_dest = fetched_instr_code[1]>>4;
    instruction->reg_source = fetched_instr_code[1] & 0x0F;

    next_instruction = (void*) instruction;
    next_instruction_format = 0;
    next_instruction_size_in_bytes = fetched_instruction->word_size*WORD_BYTES;
}

int decode_form_2_instr()
{
    format_2_instr_t instruction* = malloc(sizeof(format_2_instr_t));
    instruction->value = fetched_instr_code[0];
    
    instruction->reg_dest = fetched_instr_code[1]>>4;
    
    next_instruction = (void*) instruction;
    next_instruction_format = 0;
    next_instruction_size_in_bytes = fetched_instruction->word_size*WORD_BYTES;
}

int decode_form_3_instr()
{
    format_3_instr_t instruction* = malloc(sizeof(format_3_instr_t));
    instruction->value = fetched_instr_code[0];
    
    instruction->reg_dest = fetched_instr_code[1]>>4;
    instruction->num = ((int) fetched_instr_code[2])<<8;
    instruction->num += fetched_instr_code[3];
    
    next_instruction = (void*) instruction;
    next_instruction_format = 0;
    next_instruction_size_in_bytes = fetched_instruction->word_size*WORD_BYTES;
}

int decode_form_0_instr()
{
    format_0_instr_t instruction* = malloc(sizeof(format_0_instr_t));
    instruction->value = fetched_instr_code[0];
    
    next_instruction = (void*) instruction;
    next_instruction_format = 0;
    next_instruction_size_in_bytes = fetched_instruction->word_size*WORD_BYTES;
}

int decode_form_0_instr()
{
    format_0_instr_t instruction* = malloc(sizeof(format_0_instr_t));
    instruction->value = fetched_instr_code[0];
    
    next_instruction = (void*) instruction;
    next_instruction_format = 0;
    next_instruction_size_in_bytes = fetched_instruction->word_size*WORD_BYTES;
}

int decode_form_0_instr()
{
    format_0_instr_t instruction* = malloc(sizeof(format_0_instr_t));
    instruction->value = fetched_instr_code[0];
    
    next_instruction = (void*) instruction;
    next_instruction_format = 0;
    next_instruction_size_in_bytes = fetched_instruction->word_size*WORD_BYTES;
}
