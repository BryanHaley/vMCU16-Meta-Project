#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "interpreter.h"

#define MAX_8 255
#define MAX_16 65535
#define REG_BUFF_SIZE 54
#define NUM_GEN_REG 16

int execute_form_0_instr();
int execute_form_1_instr();
int execute_form_2_instr();
int execute_form_3_instr();
int execute_form_4_instr();
int execute_form_5_instr();
int execute_form_6_instr();
int execute_form_7_instr();
int execute_syscall_instr();

//Got weird errors using non-pointers. Fix later.
uint16_t* utemp;
int16_t* stemp;
int16_t* stemp_2;
uint32_t* utemp_32;
int32_t* stemp_32;

int execute()
{
    //ugly, ugly hack. Fix later
    utemp    = (uint16_t*) calloc(1, sizeof(uint16_t));
    stemp    = (int16_t*) calloc(1, sizeof(int16_t));
    stemp_2  = (int16_t*) calloc(1, sizeof(int16_t));
    utemp_32 = (uint32_t*) calloc(1, sizeof(uint32_t));
    stemp_32 = (int32_t*) calloc(1, sizeof(int32_t));

    switch (next_instruction_format)
    {
        case 0: execute_form_0_instr(); break;
        case 1: execute_form_1_instr(); break;
        case 2: execute_form_2_instr(); break;
        case 3: execute_form_3_instr(); break;
        case 4: execute_form_4_instr(); break;
        case 5: execute_form_5_instr(); break;
        case 6: execute_form_6_instr(); break;
        case 7: execute_form_7_instr(); break;

        case SYS_CALL_INSTR: execute_syscall_instr(); break;
    }
    
    free(utemp);   utemp = NULL;
    free(stemp);   stemp = NULL;
    free(stemp_2); stemp_2 = NULL;
    free(utemp_32); utemp_32 = NULL;
    free(stemp_32); stemp_32 = NULL;

    return NOERR;
}

int execute_form_0_instr()
{
    format_0_instr_t* instr = (format_0_instr_t*) next_instruction;

    //The only two form 0 instructions are no instruction and interrupt, so just break
    // now if it's NOP to keep things clean.

    //if (instr->value != 0xFF) { *reg_pc+= 1; return NOERR; }

    //Now assume the instruction is an interrupt

    switch (reg[0x0])
    {
        case 0x0000:
            quit = TRUE;
            break;

        case 0x0002:
            *utemp_32 = (reg[0x1]<<16)+reg[0x2];
            int buffer_len = reg[0x3];
            //for now just cheat and just print the buffer to stdout for testing purposes
            char* buffer = calloc(1, buffer_len);
            for (int i = 0; i < buffer_len; i+=2)
            { 
                buffer[i]   = (char) (main_memory[(i/WORD_BYTES)+*utemp_32]>>8);
                buffer[i+1] = (char) (main_memory[(i/WORD_BYTES)+*utemp_32]);
            }
            printf("%s", buffer);
            free(buffer); buffer = NULL;
            break;
    }

    *reg_pc += 1;
    return NOERR;
}

int execute_form_1_instr()
{
    format_1_instr_t* instr = (format_1_instr_t*) next_instruction;

    switch (instr->value)
    {
        case 0x01:
            reg[instr->reg_dest] = reg[instr->reg_source];
            break;

        case 0x03:
            reg[instr->reg_dest] += reg[instr->reg_source];
            if (((int) reg[instr->reg_dest]) + reg[instr->reg_source] > MAX_16)
            { *reg_cc = 0x01; }
            else
            { *reg_cc = 0x00; }
            break;

        case 0x04:
            reg[instr->reg_dest] += reg[instr->reg_source]+*(reg_cc);
            break;

        case 0x05:
            reg[instr->reg_dest] -= reg[instr->reg_source];
            if (((int) reg[instr->reg_dest]) - reg[instr->reg_source] < 0)
            { *reg_cc = 0x01; }
            else
            { *reg_cc = 0x00; }
            break;

        case 0x06:
            reg[instr->reg_dest] -= reg[instr->reg_source]+*(reg_cc);
            break;

        case 0x07:
            reg[instr->reg_dest] *= reg[instr->reg_source];
            if (((int) reg[instr->reg_dest]) * reg[instr->reg_source] > MAX_16)
            { *reg_cc = (((int) reg[instr->reg_dest])*reg[instr->reg_source])>>16; }
            break;

        case 0x08:
            if (reg[instr->reg_source] == 0)
            { reg[instr->reg_dest] = 0; *reg_cc = 0x01; }
            else
            { reg[instr->reg_dest] /= reg[instr->reg_source]; }
            break;

        case 0x09:
            reg[instr->reg_dest] = (uint16_t) (((int) reg[instr->reg_dest]) * 
                                                ((int) reg[instr->reg_source]));
            break;

        case 0x0a:
            reg[instr->reg_dest] = (uint16_t) (((int) reg[instr->reg_dest]) / 
                                                ((int) reg[instr->reg_source]));
            break;

        case 0x0b:
            reg[instr->reg_dest] &= reg[instr->reg_source];
            break;

        case 0x0c:
            reg[instr->reg_dest] |= reg[instr->reg_source];
            break;

        case 0x0d:
            reg[instr->reg_dest] ^= reg[instr->reg_source];
            break;

        case 0x0e: //implement bitwise NOT later
           // reg[instr->reg_dest] = reg[instr->reg_source]~reg[instr->reg_source];
            break;

        case 0x0f:
            reg[instr->reg_dest] = reg[instr->reg_dest]>>reg[instr->reg_source];
            break;
        
        case 0x10:
            reg[instr->reg_dest] = reg[instr->reg_dest]<<reg[instr->reg_source];

        case 0x11:
            *utemp = (uint16_t) (reg[instr->reg_dest]) & 0x8000; //0b1000000000000000
            reg[instr->reg_dest] = (reg[instr->reg_dest]>>reg[instr->reg_source]) | *utemp;
            break;
        
        case 0x12:
            *utemp = (uint16_t) (reg[instr->reg_dest])>>15;
            reg[instr->reg_dest] = (reg[instr->reg_dest]<<reg[instr->reg_source]) | *utemp;
            break;

        case 0x13:
            *utemp = ( (uint16_t) (reg[instr->reg_dest]) & 0x01)<<16;
            reg[instr->reg_dest] = (reg[instr->reg_dest]>>reg[instr->reg_source]) & *utemp;
            break;

        case 0x14:
            *utemp = (uint16_t) (reg[instr->reg_dest])>>15;
            reg[instr->reg_dest] = (reg[instr->reg_dest]<<reg[instr->reg_source]) & *utemp;
            break;

        case 0x19:
            if      (reg[instr->reg_dest] > reg[instr->reg_source]) { *reg_ff = 0x0001; }
            else if (reg[instr->reg_dest] < reg[instr->reg_source]) { *reg_ff = 0x0002; }
            else                                                    { *reg_ff = 0x0000; }
            break;

        case 0x1b:
            *stemp   = (int16_t) (reg[instr->reg_dest]);
            *stemp_2 = (int16_t) (reg[instr->reg_source]);
            if      (*stemp > *stemp_2) { *reg_ff = 0x0001; }
            else if (*stemp < *stemp_2) { *reg_ff = 0x0002; }
            else                        { *reg_ff = 0x0000; }
            break;

        case 0x1d:
            *stemp = (int16_t) reg[instr->reg_source];
            if      (reg[instr->reg_dest] > *stemp) { *reg_ff = 0x0001; }
            else if (reg[instr->reg_dest] < *stemp) { *reg_ff = 0x0002; }
            else                                    { *reg_ff = 0x0000; }
            break;

        //need hardware protection checks here in the future
        case 0x20:
            reg_pc[0] = reg[instr->reg_dest];
            reg_pc[1] = reg[instr->reg_source];
            break;

        case 0x2f:
            reg_bp[0] = reg[instr->reg_dest];
            reg_bp[1] = reg[instr->reg_source];
            break;

        case 0x30:
            reg[instr->reg_dest]   = reg_bp[0];
            reg[instr->reg_source] = reg_bp[1];

        case 0x31:
            reg_bp[0] = reg[instr->reg_dest];
            reg_bp[1] = reg[instr->reg_source];
            break;

        case 0x32:
            reg[instr->reg_dest]   = reg_bp[0];
            reg[instr->reg_source] = reg_bp[1];

        case 0xF0:
            if (*reg_pf == 0x0002)
            {
                reg[instr->reg_dest]   = reg_pc[0];
                reg[instr->reg_source] = reg_pc[1];
            }
            break;

        case 0xF1:
            if (*reg_pf == 0x0002)
            {
                reg_pc[0] = reg[instr->reg_dest];
                reg_pc[1] = reg[instr->reg_source];
            }
            break;

        case 0xF2:
            if (*reg_pf == 0x0002)
            {
                uint32_t address = ((uint32_t) reg[instr->reg_dest])^reg[instr->reg_source];
                for (int i = 0; i < NUM_GEN_REG; i++)
                {
                    main_memory[address+i] = reg[i];
                }
                main_memory[address+17] = *(reg_pc);
                main_memory[address+18] = *(reg_pc+1);
                main_memory[address+19] = *(reg_ff);
                main_memory[address+20] = *(reg_cc);
                main_memory[address+21] = *(reg_sp);
                main_memory[address+22] = *(reg_sp+1);
                main_memory[address+23] = *(reg_bp);
                main_memory[address+24] = *(reg_bp+1);
                main_memory[address+25] = *(reg_fp);
                main_memory[address+26] = *(reg_fp+1);
                main_memory[address+27] = *(reg_pf);

            }
            break;

        case 0xF3:
            if (*reg_pf == 0x0002)
            {
                uint32_t address = ((uint32_t) reg[instr->reg_dest])^reg[instr->reg_source];
                for (int i = 0; i < NUM_GEN_REG; i++)
                {
                    reg[i] = main_memory[address+i];
                }
                *(reg_pc)   = main_memory[address+17];
                *(reg_pc+2) = main_memory[address+18];
                *(reg_ff)   = main_memory[address+19];
                *(reg_cc)   = main_memory[address+20];
                *(reg_sp)   = main_memory[address+21];
                *(reg_sp+2) = main_memory[address+22];
                *(reg_bp)   = main_memory[address+23];
                *(reg_bp+2) = main_memory[address+24];
                *(reg_fp)   = main_memory[address+25];
                *(reg_fp+2) = main_memory[address+26];
                *(reg_pf)   = main_memory[address+27];

            }
            break;

        case 0xF4:
            if (*reg_pf == 0x0002)
            {
                reg[instr->reg_dest]   = reg_fp[0];
                reg[instr->reg_source] = reg_fp[1];
            }
            break;

        case 0xF5:
            if (*reg_pf == 0x0002)
            {
                reg_fp[0] = reg[instr->reg_dest];
                reg_fp[1] = reg[instr->reg_source];
            }
            break;
    }

    *reg_pc += 1;
    return NOERR;
}

int execute_form_2_instr()
{
    format_2_instr_t* instr = (format_2_instr_t*) next_instruction;

    switch (instr->value)
    {
        case 0x2b:
            if (((uint32_t)reg[instr->reg_dest]+1) > MAX_16)
            { *reg_cc = 0x0001; }
            else 
            { *reg_cc = 0x0000; }

            reg[instr->reg_dest] += 1;
            break;

        case 0x2c:
            if (((int32_t)reg[instr->reg_dest]-1) < 0)
            { *reg_cc = 0x0001; }
            else 
            { *reg_cc = 0x0000; }

            reg[instr->reg_dest] -= 1;
            break;

        case 0x2d:
            *utemp_32 = ((uint32_t) reg_bp[0])+reg_bp[1]+1;
            main_memory[*utemp_32+reg_sp[0]+reg_sp[1]] = reg[instr->reg_dest];
            reg_bp[0] = (*utemp_32)>>16;
            reg_bp[1] = (uint16_t) *utemp_32;
            break;

        case 0x2e:
            *utemp_32 = ((uint32_t) reg_bp[0])+reg_bp[1]-1;
            reg[instr->reg_dest] = main_memory[*utemp_32+reg_sp[0]+reg_sp[1]];
            reg_bp[0] = (*utemp_32)>>16;
            reg_bp[1] = (uint16_t) utemp_32;
            break;           
    }

    *reg_pc += 1;
    return NOERR;
}

int execute_form_3_instr()
{
    format_3_instr_t* instr = (format_3_instr_t*) next_instruction;

    switch (instr->value)
    {
        case 0x02:
            reg[instr->reg_dest] = instr->num;
            break;

        case 0x1a:
            if      (reg[instr->reg_dest] == instr->num) { *reg_ff = 0x0000; }
            else if (reg[instr->reg_dest] > instr->num)  { *reg_ff = 0x0001; }
            else                                         { *reg_ff = 0x0002; }
            break;

        case 0x1c:
            *stemp = (int16_t) (reg[instr->reg_dest]);
            *stemp_2 = (int16_t) (instr->num);
            if      (*stemp == *stemp_2) { *reg_ff = 0x0000; }
            else if (*stemp > *stemp_2)  { *reg_ff = 0x0001; }
            else                         { *reg_ff = 0x0002; }
            break;

        case 0x1e:
            *stemp = (int16_t) (reg[instr->reg_dest]);
            if      (*stemp == instr->num) { *reg_ff = 0x0000; }
            else if (*stemp > instr->num)  { *reg_ff = 0x0001; }
            else                           { *reg_ff = 0x0002; }
            break;
    }

    *reg_pc += 2;
    return NOERR;
}

int execute_form_4_instr()
{
    format_4_instr_t* instr = (format_4_instr_t*) next_instruction;

    switch (instr->value)
    {
        case 0x15:
            /*if (instr->relocatable)
            { instr->address = main_memory[address]; }*/
            
            instr->address = main_memory[instr->address]; 

            switch (instr->msb_switch)
            {
                case 0x00: instr->address = instr->address & 0x00FF; break;
                case 0x01: instr->address = instr->address>>8; break;
            }
        
            reg[instr->reg_dest] ^= instr->address;
            break;

        case 0x16:
            *utemp = reg[instr->reg_dest];

            switch (instr->msb_switch)
            {
                case 0x00: *utemp = (*utemp) & 0x00FF; break;
                case 0x01: *utemp = (*utemp)>>8; break;
            }

            main_memory[instr->address] ^= *utemp;
            break;
    }

    *reg_pc += 3;
    return NOERR;
}

int execute_form_5_instr()
{
    format_5_instr_t* instr = (format_5_instr_t*) next_instruction;

    switch (instr->value)
    {
        case 0x17:
            *utemp = main_memory[((uint32_t) reg[instr->reg_msw])+
                                            reg[instr->reg_lsw]];

            switch (instr->msb_switch)
            {
                case 0x00: *utemp = (*utemp) & 0x00FF; break;
                case 0x01: *utemp = (*utemp)>>8; break;
            }

            reg[instr->reg_dest] ^= *utemp;
            break;

        case 0x18:
            *utemp = reg[instr->reg_dest];

            switch (instr->msb_switch)
            {
                case 0x00: *utemp = (*utemp) & 0x00FF; break;
                case 0x01: *utemp = (*utemp)>>8;
            }

            main_memory[((uint32_t) reg[instr->reg_msw])+reg[instr->reg_lsw]] ^= 
                        (*utemp);
    }

    *reg_pc += 2;
    return NOERR;
}

int execute_form_6_instr()
{
    format_6_instr_t* instr = (format_6_instr_t*) next_instruction;

    int jump = FALSE;

    switch (instr->value)
    {
        case 0x1f:
            jump = TRUE; break;
        
        case 0x21:
            if (*reg_ff == 0x00)
            { jump = TRUE; }
            break;

        case 0x22:
            if (*reg_ff != 0x00)
            { jump = TRUE; }
            break;

        case 0x23:
            if (*reg_ff == 0x01)
            { jump = TRUE; }
            break;

        case 0x24:
            if (*reg_ff == 0x02)
            { jump = TRUE; }
            break;

        case 0x25:
            if (*reg_ff == 0x00 || *reg_ff == 0x02)
            { jump = TRUE; }
            break;

        case 0x26:
            if (*reg_ff == 0x00 || *reg_ff == 0x01)
            { jump = TRUE; }
            break;

        case 0x27:
            if (*reg_cc != 0x00)
            { jump = TRUE; }
            break;

        case 0x28:
            if (*reg_cc == 0x00)
            { jump = TRUE; }
            break;

        case 0x29:
            reg_fp[0] = reg_pc[0];
            reg_fp[1] = reg_fp[1];
            jump = TRUE;
            break;
    }

    if (jump)
    {
        reg_pc[0] = instr->address>>16;
        reg_pc[1] = (uint16_t) instr->address;
    }

    else { *reg_pc += 3; }
    return NOERR;
}

int execute_form_7_instr()
{
    format_7_instr_t* instr = (format_7_instr_t*) next_instruction;

    switch (instr->value)
    {
        case 0x33:
            *stemp_32 = (int32_t) instr->num;
            instr->num = reg_sp[0]+reg_sp[1]+reg_bp[0]+reg_bp[1]+(*stemp_32);
            reg[instr->reg_dest] = instr->num>>16;
            reg[instr->reg_dest+1] = (uint16_t) instr->num; //need error checking here
            break;

        case 0x34:
            reg[instr->reg_dest] = instr->num>>16;
            reg[instr->reg_dest+1] = (uint16_t) instr->num;
            break;
    }

    *reg_pc += 3;
    return NOERR;
}

int execute_syscall_instr()
{
    //do later

    *reg_pc += 2;
    return NOERR;
}
