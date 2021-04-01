/*
 * Femto, a fictive computer emulator
 * Copyright (C) 2021 Semperfis
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

/* Computer architecture:
 * - 4KBs RAM
 * - RISC CPU: 4 GP REGISTERS; INTEGER ONLY; REDUCE ADDRESSING MODES & MEMORY
 * - STRUCTURE OF FLAGS REGISTER: XXXX XNCZ (N : Negative, C : Carry; Z : Zero)
 * - INSTRUCTION FORMAT: (I: INST; M : ADDRESSING MODES; R : REGISTERS; D : DATA; A : ADDRESS)
 * - IIII IIIM   RRRR xxxx   DDDD DDDD
 * - IIII IIIM   RRRR AAAA   AAAA AAAA
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef _ASM_
    #define _ASM_
    #include "common.h"
#endif

#define DISM_BUFFER 32


/*** CODE BEGINNING ***/
void cmd_help(void)
{
    printf("Usage: dism [OPTION]\n");
    printf("Run the femto disassembler with INFILE as binary input, configure with OPTION.\n\n");
    printf("--help             : display this help & exit\n");
    printf(" -h | -?\n");
    printf("--version          : output version information and exit\n");
    printf(" -v\n");
    printf("--file [INFILE]    : specify the file to be disassemble\n");
    printf(" -f\n");
}

void cmd_version(void)
{
    printf("dism %s | Copyright (C) 2021 Semperfis\n", (char *)DISM_VERSION);
    printf("This program comes with ABSOLUTELY NO WARRANTY;\n");
    printf("License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>;\n");
    printf("This is free software, and you are welcome to redistribute it under certain conditions;\n");
}


const char * disasm_inst(uint8_t byte)
{
    int i = 0;

    /* FIND THE INSTRUCTION IN THE TRANSLATION TABLE */
    while (inst_trans_table[i].value != byte)
    {
        i++;
    }

    return inst_trans_table[i].str;
}


const char * disasm_dest(uint8_t byte)
{
    return '\0';
}


const char * disasm_src(uint8_t byte)
{
    return '\0';
}


void disasm(uint8_t *buffer, uint16_t pc, char *result)
{
    uint8_t f[3] = {0};

    /* RESET RESULT STRING */
    memset((void *)result, 0, DISM_BUFFER);

    f[0] = buffer[pc];
    f[1] = buffer[(pc + 1)];
    f[2] = buffer[(pc + 2)];

    strncat(result, disasm_inst(f[0]), (size_t)DISM_BUFFER);
    //strncat(result, disasm_dest(f[1]), (size_t)DISM_BUFFER);
    //strncat(result, disasm_src(f[2]),  (size_t)DISM_BUFFER);
}


int main(int argc, char *argv[])
{
    char    *src_fname = NULL;
    FILE    *src_file  = NULL;
    uint8_t *src_bin   = NULL;
    long     src_size  = 0L;
    char    *result    = NULL;


    /*** COMMAND-LINE ARGUMENTS ***/
    if (argc == 1)
    {
        cmd_help();
        return 0;
    }

    for (int i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "-?") == 0)
        {
            cmd_help();
            return 0;
        }
        else if (strcmp(argv[i], "--version") == 0 || strcmp(argv[i], "-v") == 0)
        {
            cmd_version();
            return 0;
        }
        else if (strcmp(argv[i], "--file") == 0 || strcmp(argv[i], "-f") == 0)
        {
            i++;
            src_fname = argv[i];
        }
    }


    /* ALLOCATE MEMORY FOR RESULT STRING */
    result = malloc(DISM_BUFFER * sizeof(char));
    if (result == NULL)
    {
        printf("ERROR (main) : CAN'T ALLOCATE MEM FOR BUFFER !!!\n");
        return -1;
    }


    /*** FILE LOADING ***/
    src_file = fopen(src_fname, "rb");
    if (src_file == NULL)
    {
        printf("ERROR (main): CAN'T OPEN FILE \"%s\" !!!\n", src_fname);
        exit(-1);
    }

    /* GET FILE SIZE */
    fseek(src_file, 0L, SEEK_END);
    src_size = ftell(src_file);
    fseek(src_file, 0L, SEEK_SET);

    /* ALLOC MEMORY FOR THE BUFFER */
    src_bin = malloc((int)src_size * sizeof(uint8_t));
    if (src_bin == NULL)
    {
        printf("ERROR (main) : CAN'T ALLOC MEM FOR BUFFER !!!\n");
        fclose(src_file);
        exit(-1);
    }

    /* READ BINARY FILE*/
    if (fread((void *)src_bin, sizeof(uint8_t), (size_t)src_size, src_file) != (size_t)src_size)
    {
        printf("ERROR (main): CAN'T READ PROPERLY THE FILE \"%s\" !!!\n", src_fname);
        fclose(src_file);
        exit(-1);
    }

    fclose(src_file);


    /*** DISASSEMBLING ***/
    for (int i = 0; i < src_size; i += 3)
    {
        disasm(src_bin, i, result);
        printf("%03X : %02X%02X%02X\t%s\n", i, src_bin[i], src_bin[i+1], src_bin[i+2], result);
    }

    return 0;
}