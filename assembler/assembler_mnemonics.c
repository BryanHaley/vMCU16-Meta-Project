#include <stdlib.h>
#include <string.h>
#include "assembler.h"
#include "assembler_mnemonics.h"

// Populates an array with all of the valid mnemonics the assembler will recognize
// Probably a better way to do this, investigate later.
int initialize_valid_mnemonics()
{
    valid_mnemonics = (mnemonic*) calloc (NUM_MNEMONICS, sizeof(mnemonic));

    //Set some defaults to avoid a bit of redundancy below
    for (int i = 0; i < NUM_MNEMONICS; i++)
    {
        valid_mnemonics[i].alias = "ERROR";
        valid_mnemonics[i].value = 0x00;
        valid_mnemonics[i].word_size = 1;
        valid_mnemonics[i].num_operands = 2;
        valid_mnemonics[i].format_type = 1; // see cpu_desc.txt
    }

    valid_mnemonics[0].alias      = "NOP";
    valid_mnemonics[0].value      = 0x00;
    valid_mnemonics[0].format_type = 0;

    valid_mnemonics[1].alias       = "SET";
    valid_mnemonics[1].value       = 0x00;
    valid_mnemonics[1].word_size   = PREPROCESSOR_INSTRUCTION;
    valid_mnemonics[1].format_type = 8;

    valid_mnemonics[2].alias     = "SER";
    valid_mnemonics[2].value     = 0x01;

    valid_mnemonics[3].alias       = "SEW";
    valid_mnemonics[3].value       = 0x02;
    valid_mnemonics[3].word_size   = 2;
    valid_mnemonics[3].format_type = 3;

    valid_mnemonics[4].alias     = "ADD";
    valid_mnemonics[4].value     = 0x02;

    valid_mnemonics[5].alias     = "ADC";
    valid_mnemonics[5].value     = 0x04;

    valid_mnemonics[6].alias     = "ADDC";
    valid_mnemonics[6].value     = 0x04;

    valid_mnemonics[7].alias     = "SUB";
    valid_mnemonics[7].value     = 0x05;

    valid_mnemonics[8].alias     = "SBB";
    valid_mnemonics[8].value     = 0x06;

    valid_mnemonics[9].alias     = "SUBB";
    valid_mnemonics[9].value     = 0x06;

    valid_mnemonics[10].alias     = "MUL";
    valid_mnemonics[10].value     = 0x07;

    valid_mnemonics[11].alias     = "DIV";
    valid_mnemonics[11].value     = 0x08;

    valid_mnemonics[12].alias     = "MLS";
    valid_mnemonics[12].value     = 0x09;

    valid_mnemonics[13].alias     = "MULS";
    valid_mnemonics[13].value     = 0x09;

    valid_mnemonics[14].alias     = "DVS";
    valid_mnemonics[14].value     = 0x0a;

    valid_mnemonics[15].alias     = "DIVS";
    valid_mnemonics[15].value     = 0x0a;

    valid_mnemonics[16].alias     = "AND";
    valid_mnemonics[16].value     = 0x0b;

    valid_mnemonics[17].alias     = "BOR";
    valid_mnemonics[17].value     = 0x0c;

    valid_mnemonics[18].alias     = "OR";
    valid_mnemonics[18].value     = 0x0c;

    valid_mnemonics[19].alias     = "XOR";
    valid_mnemonics[19].value     = 0x0d;

    valid_mnemonics[20].alias     = "NOT";
    valid_mnemonics[20].value     = 0x0e;

    valid_mnemonics[21].alias     = "SHR";
    valid_mnemonics[21].value     = 0x0f;

    valid_mnemonics[22].alias     = "SHL";
    valid_mnemonics[22].value     = 0x10;

    valid_mnemonics[23].alias     = "ASR";
    valid_mnemonics[23].value     = 0x11;

    valid_mnemonics[23].alias     = "ASL";
    valid_mnemonics[23].value     = 0x12;

    valid_mnemonics[23].alias     = "ROR";
    valid_mnemonics[23].value     = 0x13;

    valid_mnemonics[24].alias     = "ROL";
    valid_mnemonics[24].value     = 0x14;

    valid_mnemonics[25].alias       = "CLR";
    valid_mnemonics[25].value       = 0x00;
    valid_mnemonics[25].word_size   = PREPROCESSOR_INSTRUCTION;
    valid_mnemonics[25].format_type = 8; 

    valid_mnemonics[26].alias       = "LOD";
    valid_mnemonics[26].value       = 0x15;
    valid_mnemonics[26].word_size   = 3;
    valid_mnemonics[26].format_type = 4;

    valid_mnemonics[27].alias       = "LOAD";
    valid_mnemonics[27].value       = 0x15;
    valid_mnemonics[27].word_size   = 3;
    valid_mnemonics[27].format_type = 4;

    valid_mnemonics[28].alias       = "STR";
    valid_mnemonics[28].value       = 0x16;
    valid_mnemonics[28].word_size   = 3;
    valid_mnemonics[28].format_type = 4; 

    valid_mnemonics[29].alias        = "LAA";
    valid_mnemonics[29].value        = 0x17;
    valid_mnemonics[29].word_size    = 2;
    valid_mnemonics[29].num_operands = 3;
    valid_mnemonics[29].format_type  = 5;

    valid_mnemonics[30].alias        = "LODAADR";
    valid_mnemonics[30].value        = 0x17;
    valid_mnemonics[30].word_size    = 2;
    valid_mnemonics[30].num_operands = 3;
    valid_mnemonics[30].format_type  = 5;

    valid_mnemonics[31].alias        = "LOADAADR";
    valid_mnemonics[31].value        = 0x17;
    valid_mnemonics[31].word_size    = 2;
    valid_mnemonics[31].num_operands = 3;
    valid_mnemonics[31].format_type  = 5;

    valid_mnemonics[32].alias        = "SAA";
    valid_mnemonics[32].value        = 0x18;
    valid_mnemonics[32].word_size    = 2;
    valid_mnemonics[32].num_operands = 3;
    valid_mnemonics[32].format_type  = 5;

    valid_mnemonics[33].alias        = "STRAADR";
    valid_mnemonics[33].value        = 0x18;
    valid_mnemonics[33].word_size    = 2;
    valid_mnemonics[33].num_operands = 3;
    valid_mnemonics[33].format_type  = 5;

    valid_mnemonics[34].alias       = "CMP";
    valid_mnemonics[34].value       = 0x00;
    valid_mnemonics[34].word_size   = PREPROCESSOR_INSTRUCTION;
    valid_mnemonics[34].format_type = 8;

    valid_mnemonics[35].alias       = "CMR";
    valid_mnemonics[35].value       = 0x19;

    valid_mnemonics[36].alias       = "CMW";
    valid_mnemonics[36].value       = 0x1a;
    valid_mnemonics[36].word_size   = 2;
    valid_mnemonics[36].format_type = 3;

    valid_mnemonics[37].alias       = "CMS";
    valid_mnemonics[37].value       = 0x00;
    valid_mnemonics[37].word_size   = PREPROCESSOR_INSTRUCTION;
    valid_mnemonics[37].format_type = 8;

    valid_mnemonics[38].alias       = "CMPS";
    valid_mnemonics[38].value       = 0x00;
    valid_mnemonics[38].word_size   = PREPROCESSOR_INSTRUCTION;
    valid_mnemonics[38].format_type = 8;

    valid_mnemonics[39].alias     = "CSR";
    valid_mnemonics[39].value     = 0x1b;

    valid_mnemonics[40].alias       = "CSW";
    valid_mnemonics[40].value       = 0x1c;
    valid_mnemonics[40].word_size   = 2;
    valid_mnemonics[40].format_type = 3;

    valid_mnemonics[41].alias       = "CUS";
    valid_mnemonics[41].value       = 0x00;
    valid_mnemonics[41].word_size   = PREPROCESSOR_INSTRUCTION;
    valid_mnemonics[41].format_type = 8;

    valid_mnemonics[42].alias       = "CMPUS";
    valid_mnemonics[42].value       = 0x00;
    valid_mnemonics[42].word_size   = PREPROCESSOR_INSTRUCTION;
    valid_mnemonics[42].format_type = 8;

    valid_mnemonics[43].alias     = "CUR";
    valid_mnemonics[43].value     = 0x1d;

    valid_mnemonics[44].alias       = "CUW";
    valid_mnemonics[44].value       = 0x1e;
    valid_mnemonics[44].word_size   = 2;
    valid_mnemonics[44].format_type = 3;

    valid_mnemonics[45].alias       = "JMP";
    valid_mnemonics[45].value       = 0x1f;
    valid_mnemonics[45].word_size   = 3;
    valid_mnemonics[45].format_type = 6;

    valid_mnemonics[46].alias     = "JMA";
    valid_mnemonics[46].value     = 0x20;
    valid_mnemonics[46].word_size = 3;

    valid_mnemonics[47].alias     = "JAM";
    valid_mnemonics[47].value     = 0x20;
    valid_mnemonics[47].word_size = 3;

    valid_mnemonics[48].alias     = "JMPADR";
    valid_mnemonics[48].value     = 0x20;
    valid_mnemonics[48].word_size = 3;

    valid_mnemonics[49].alias     = "JMPA";
    valid_mnemonics[49].value     = 0x20;
    valid_mnemonics[49].word_size = 3;

    valid_mnemonics[50].alias     = "JIE";
    valid_mnemonics[50].value     = 0x21;
    valid_mnemonics[50].word_size = 3;
    valid_mnemonics[50].format_type = 6;

    valid_mnemonics[51].alias     = "JE";
    valid_mnemonics[51].value     = 0x21;
    valid_mnemonics[51].word_size = 3;
    valid_mnemonics[51].format_type = 6;

    valid_mnemonics[52].alias     = "JNE";
    valid_mnemonics[52].value     = 0x22;
    valid_mnemonics[52].word_size = 3;
    valid_mnemonics[52].format_type = 6;

    valid_mnemonics[53].alias     = "JIG";
    valid_mnemonics[53].value     = 0x23;
    valid_mnemonics[53].word_size = 3;
    valid_mnemonics[53].format_type = 6;

    valid_mnemonics[54].alias     = "JIL";
    valid_mnemonics[54].value     = 0x24;
    valid_mnemonics[54].word_size = 3;
    valid_mnemonics[54].format_type = 6;

    valid_mnemonics[55].alias     = "JLE";
    valid_mnemonics[55].value     = 0x25;
    valid_mnemonics[55].word_size = 3;
    valid_mnemonics[55].format_type = 6;

    valid_mnemonics[56].alias     = "JGE";
    valid_mnemonics[56].value     = 0x26;
    valid_mnemonics[56].word_size = 3;
    valid_mnemonics[56].format_type = 6;

    valid_mnemonics[57].alias     = "JIC";
    valid_mnemonics[57].value     = 0x27;
    valid_mnemonics[57].word_size = 3;
    valid_mnemonics[57].format_type = 6;

    valid_mnemonics[58].alias     = "JC";
    valid_mnemonics[58].value     = 0x27;
    valid_mnemonics[58].word_size = 3;
    valid_mnemonics[58].format_type = 6;

    valid_mnemonics[59].alias     = "JNC";
    valid_mnemonics[59].value     = 0x28;
    valid_mnemonics[59].word_size = 3;
    valid_mnemonics[59].format_type = 6;

    valid_mnemonics[60].alias     = "CAL";
    valid_mnemonics[60].value     = 0x29;
    valid_mnemonics[60].word_size = 3;
    valid_mnemonics[60].format_type = 6;

    valid_mnemonics[61].alias     = "CALL";
    valid_mnemonics[61].value     = 0x29;
    valid_mnemonics[61].word_size = 3;
    valid_mnemonics[61].format_type = 6;

    valid_mnemonics[62].alias       = "RET";
    valid_mnemonics[62].value       = 0x2a;
    valid_mnemonics[62].format_type = 0;

    valid_mnemonics[63].alias       = "INC";
    valid_mnemonics[63].value       = 0x2b;
    valid_mnemonics[63].format_type = 2;

    valid_mnemonics[64].alias       = "DEC";
    valid_mnemonics[64].value       = 0x2c;
    valid_mnemonics[64].format_type = 2;

    valid_mnemonics[65].alias       = "PSH";
    valid_mnemonics[65].value       = 0x2d;
    valid_mnemonics[65].format_type = 2;

    valid_mnemonics[66].alias       = "PUSH";
    valid_mnemonics[66].value       = 0x2d;
    valid_mnemonics[66].format_type = 2;

    valid_mnemonics[67].alias       = "POP";
    valid_mnemonics[67].value       = 0x2e;
    valid_mnemonics[67].format_type = 2;

    valid_mnemonics[68].alias     = "SBP";
    valid_mnemonics[68].value     = 0x2f;

    valid_mnemonics[69].alias     = "LBP";
    valid_mnemonics[69].value     = 0x30;

    valid_mnemonics[70].alias     = "SSP";
    valid_mnemonics[70].value     = 0x31;

    valid_mnemonics[71].alias     = "LSP";
    valid_mnemonics[71].value     = 0x32;

    valid_mnemonics[72].alias        = "LSA";
    valid_mnemonics[72].value        = 0x33;
    valid_mnemonics[72].word_size    = 3;
    valid_mnemonics[72].num_operands = 3;
    valid_mnemonics[72].format_type = 7;

    valid_mnemonics[73].alias        = "LODSA";
    valid_mnemonics[73].value        = 0x33;
    valid_mnemonics[73].word_size    = 3;
    valid_mnemonics[73].num_operands = 3;
    valid_mnemonics[73].format_type = 7;

    valid_mnemonics[74].alias        = "LODSADR";
    valid_mnemonics[74].value        = 0x33;
    valid_mnemonics[74].word_size    = 3;
    valid_mnemonics[74].num_operands = 3;
    valid_mnemonics[74].format_type = 7;

    valid_mnemonics[75].alias        = "S32";
    valid_mnemonics[75].value        = 0x34;
    valid_mnemonics[75].word_size    = 3;
    valid_mnemonics[75].num_operands = 3;
    valid_mnemonics[75].format_type = 7;

    valid_mnemonics[76].alias        = "SET32";
    valid_mnemonics[76].value        = 0x34;
    valid_mnemonics[76].word_size    = 3;
    valid_mnemonics[76].num_operands = 3;
    valid_mnemonics[76].format_type = 7;

    valid_mnemonics[77].alias        = "SDW";
    valid_mnemonics[77].value        = 0x34;
    valid_mnemonics[77].word_size    = 3;
    valid_mnemonics[77].num_operands = 3;
    valid_mnemonics[77].format_type = 7;

    valid_mnemonics[78].alias     = "SYS";
    valid_mnemonics[78].value     = 0xEF;
    valid_mnemonics[78].word_size = 2;
    valid_mnemonics[78].format_type = 8;

    valid_mnemonics[79].alias     = "LPC";
    valid_mnemonics[79].value     = 0xF0;

    valid_mnemonics[80].alias     = "SPC";
    valid_mnemonics[80].value     = 0xF1;

    valid_mnemonics[81].alias     = "SVR";
    valid_mnemonics[81].value     = 0xF2;

    valid_mnemonics[82].alias     = "RSR";
    valid_mnemonics[82].value     = 0xF3;

    valid_mnemonics[83].alias     = "LFP";
    valid_mnemonics[83].value     = 0xF4;

    valid_mnemonics[84].alias     = "SFP";
    valid_mnemonics[84].value     = 0xF5;

    valid_mnemonics[85].alias       = "INT";
    valid_mnemonics[85].value       = 0xFF;
    valid_mnemonics[85].format_type = 0;

    return NOERR;
}

mnemonic* get_mnemonic_by_alias(char* alias)
{
    //Quick and dirty search; optimize later.
    for (int i = 0; i < NUM_MNEMONICS; i++)
    {
        if (strcmp(valid_mnemonics[i].alias, alias) == 0)
        { return &valid_mnemonics[i]; }
    }

    return NULL;
}

int is_valid_mnemonic(char* alias)
{
    if (alias[0] == '.') //shortcut to avoid unnecessary search
    { return FALSE; }

    if (get_mnemonic_by_alias(alias) != NULL)
    { return TRUE; }

    return FALSE;
}
