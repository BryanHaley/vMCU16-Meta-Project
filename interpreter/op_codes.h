#ifndef OP_CODES_H
#define OP_CODES_H

#define NUM_INSTRUCTIONS 81

typedef struct
{
    int word_size;
    int format_type;
} op_instr;

op_instr* valid_instructions;
int initialize_valid_instructions();
op_instr* get_instruction_by_value(uint8_t value);

#endif
