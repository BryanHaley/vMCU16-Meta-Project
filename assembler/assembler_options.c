#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "assembler_options.h"

#define NOERR 0
#define ERR -1

int initialize_default_options()
{
    assembler_keep_isf  = (int*) calloc(1, sizeof(int));
    *assembler_keep_isf = FALSE;

    return NOERR;
}

int parse_option(char* str)
{
    if (strncmp(str, "--keep-isf", strlen(str)) == 0 || 
        strncmp(str, "-kisf", strlen(str)) == 0)
    {
        *assembler_keep_isf = TRUE;
        return NOERR;
    }

    fprintf(stderr, "Unrecognized option %s\n", str);
    
    return ERR;
}
