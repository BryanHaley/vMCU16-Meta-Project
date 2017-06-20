#include <stdlib.h>
#include "op_codes.h"

int initialize_valid_instructions()
{
    valid_instructions = (op_instr*) calloc (NUM_INSTRUCTIONS, sizeof(op_isntr));

    for (int i = 0; i < NUM_INSTRUCTIONS; i++)
    {
        valid_instructions[i].word_size = 1;
        valid_instructions[i].format_type = 1; // see cpu_desc.txt
    }

    valid_instructions[0x00].format_type = 0;
    valid_instructions[0x02].format_type = 3;
    valid_instructions[0x15].format_type = 4;
    valid_instructions[0x16].format_type = 4;
    valid_instructions[0x17].format_type = 5;
    valid_instructions[0x18].format_type = 5;
    valid_instructions[0x1a].format_type = 3;
    valid_instructions[0x1e].format_type = 3;
    valid_instructions[0x1f].format_type = 6;
    
    for (int i = 0x21; i <= 0x29; i++)
    {
        valid_instructions[i].format_type = 6;
    }

    valid_instructions[0x2a].format_type = 0;
    valid_instructions[0x2b].format_type = 2;
    valid_instructions[0x2c].format_type = 2;
    valid_instructions[0x2d].format_type = 2;
    valid_instructions[0x2e].format_type = 2;
    valid_instructions[0x33].format_type = 7;
    valid_instructions[0x34].format_type = 7;

    valid_instructions[0xEF].format_type = 8;
    valid_instructions[0xEF].word_size = 2;

    valid_instructions[0xFF].format_type = 0;

    for (int i = 0; i < NUM_INSTRUCTIONS; i++)
    {
        switch (valid_instructions[i].format_type)
        {
            case 0: valid_instructions[i].word_size = 1; break;
            case 1: valid_instructions[i].word_size = 1; break;
            case 2: valid_instructions[i].word_size = 1; break;
            case 3: valid_instructions[i].word_size = 2; break;
            case 4: valid_instructions[i].word_size = 3; break;
            case 5: valid_instructions[i].word_size = 2; break;
            case 6: valid_instructions[i].word_size = 3; break;
            case 7: valid_instructions[i].word_size = 3; break;
        }
    }

    return NOERR;
}

mnemonic* get_instruction_by_value(uint8_t value)
{
    return &valid_instructions[i];
}
