#ifndef ASSEMBLER_UTILS_H
#define ASSEMBLER_UTILS_H

#include <stdint.h>

// Don't need these declared here; remove later
int read_buff;
char read_char;

// Managing opening and closing the files through functions might be unnecessary;
//  investigate later if I should just use fopen and fclose in place of them.
int open_source_file(char* filename);
int close_source_file();

int open_isf_file(char* filename);
int close_isf_file();

int open_object_file(char* filename);
int close_object_file();

// The purpose of get next word is to make dealing with whitespace and comments easy;
//  it only returns relevant text. Not to be confused with a CPU "word".
char* get_next_word();

// Definitely don't need this; just use fseek. Remove later.
int start_from_beginning();

// These ended up not being useful. Perhaps remove later.
char* get_next_line();
int skip_to_next_line();

// Ended up not using this. Delete later.
int write_words(uint16_t* words, int length);

// Some helper functions for managing taking ASCII characters and converting them into
// symbols or literals or etc.
int is_str_number(char* str);
uint32_t get_val_from_str(char* str);
uint32_t get_val_from_hex_str(char* str);
uint32_t get_val_from_hex_char(char ch);

#endif
