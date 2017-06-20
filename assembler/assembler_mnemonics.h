#ifndef ASSEMBLER_MNEMONICS_H
#define ASSEMBLER_MNEMONICS_H

#define NUM_MNEMONICS 86
#define PREPROCESSOR_INSTRUCTION -1

// This struct is used to hold info interpreted from the source file so that
// pass two may use it to generate equivalent machine code
typedef struct
{
    char* alias;
    uint8_t value;
    int word_size;
    int num_operands;
    int format_type;
} mnemonic;

// For organization purposes, all valid mnemonics are kept in array.
// Probably not efficient on memory; optimize later.
mnemonic* valid_mnemonics;
int initialize_valid_mnemonics();
mnemonic* get_mnemonic_by_alias(char* alias);
int is_valid_mnemonic(char* alias);

#endif
