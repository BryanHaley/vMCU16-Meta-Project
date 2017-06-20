#ifndef LINKER_H
#define LINKER_H

#include <stdint.h>

FILE* elf_file;
FILE* object_file;

typedef struct
{
    uint32_t magic_number;
    uint8_t bit_flag;
    uint8_t endianness;
    uint8_t version;
    uint8_t abi;
    uint32_t abi_unused;
    uint16_t type;
    uint16_t instr_set;
    uint32_t version_2;
    uint32_t entry_point;
    uint32_t header_table;
    uint32_t section_table;
    uint32_t flags;
    uint16_t header_size;
    uint16_t header_table_size;
    uint16_t header_table_entries;
    uint16_t section_table_size;
    uint16_t section_table_entries;
    uint16_t section_table_names;
} elf_header;

int open_files();
int apply_headers();
int copy_text();
int relocate_addrs();
int apply_section_header();

#endif
