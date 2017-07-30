#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <stdint.h>
#include "op_codes.h"

#define TRUE 1
#define FALSE 0
#define ERR -1
#define NOERR 0

#define NUM_REG 16
#define DOUBLE_WORD_REG 2
#define SINGLE_WORD_REG 1
#define WORD_BYTES 2

#define SYS_CALL_INSTR 8

int quit;

uint16_t* main_memory;
uint32_t memory_size;

op_instr* fetch();
int decode();
int execute();

uint8_t* fetched_instr_code;
op_instr* fetched_instruction;

int initialize_registers();

uint16_t* reg;
uint16_t* reg_pc;
uint16_t* reg_ff;
uint16_t* reg_cc;
uint16_t* reg_sp;
uint16_t* reg_bp;
uint16_t* reg_fp;
uint16_t* reg_pf;

typedef struct
{
    uint8_t value;
} format_0_instr_t;

typedef struct
{
    uint8_t value;
    int reg_dest;
    int reg_source;
} format_1_instr_t;

typedef struct
{
    uint8_t value;
    int reg_dest;
} format_2_instr_t;

typedef struct
{
    uint8_t value;
    int reg_dest;
    uint16_t num;
} format_3_instr_t;

typedef struct
{
    uint8_t value;
    int reg_dest;
    int msb_switch;
    int address_flag;
    int relocatable;
    uint32_t address;
} format_4_instr_t;

typedef struct
{
    uint8_t value;
    int reg_dest;
    int reg_dest_2;
    int reg_msw;
    int reg_lsw;
    int msb_switch;
    int address_flag;
    int relocatable;
} format_5_instr_t;

typedef struct
{
    uint8_t value;
    int address_flag;
    int relocatable;
    uint32_t address;
} format_6_instr_t;

typedef struct
{
    uint8_t value;
    int reg_dest;
    int relocatable;
    uint32_t num;
} format_7_instr_t;

//Format 8 instructions each get their own struct

typedef struct
{
    uint16_t num;
} sys_instr_t;

void* next_instruction;
int next_instruction_format;
int next_instruction_size_in_bytes;

#endif
