#ifndef ASSEMBLER_OPTIONS_H
#define ASSEMBLER_OPTIONS_H

#define TRUE 1
#define FALSE 0

int* assembler_keep_isf;

int initialize_default_options();
int parse_option(char* str);

#endif
