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
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#define ASM_VERSION "1.0"
#define ADRM_IMM false
#define ADRM_REG true


typedef enum inst
{
    HLT = 0x0,
    LDR = 0x1,
    LDM = 0x2,
    STI = 0x3,
    STR = 0x4,
    ADD = 0x5,
    SUB = 0x6,
    CMP = 0x7,
    JZ  = 0x8, /* JUMP IF ZERO; SAME AS JE */
    JN  = 0x9, /* JUMP IF NEGATIVE */
    JC  = 0xA, /* JUMP IF CARRY; SAME AS JB (JUMP IF BELOW) */
    JNC = 0xB, /* JUMP IF NOT CARRY; SAME AS JAE (JUMP IF ABOVE OR EQUAL) */
    JBE = 0xC, /* JUMP IF BELOW OR EQUAL (CARRY OR ZERO) */
    JA  = 0xD, /* JUMP IF ABOVE (!CARRY OR !ZERO) */
    JMP = 0xE
} inst_t;

typedef struct trans
{
    char    str[4];
    uint8_t value;
} trans_t;

const trans_t translate[] =
{
    {"HLT", HLT},
    {"LDR", LDR},
    {"LDM", LDM},
    {"STI", STI},
    {"STR", STR},
    {"ADD", ADD},
    {"SUB", SUB},
    {"CMP", CMP},
    {"JMP", JMP},
    {"JZ",  JZ },
    {"JN",  JN },
    {"JC",  JC },
    {"JNC", JNC},
    {"JBE", JBE},
    {"JA",  JA }
};

const trans_t registers[] =
{
    {"R0", 0x0},
    {"R1", 0x1},
    {"R2", 0x2},
    {"R3", 0x3}
};


void cmd_help(void)
{
    printf("Usage: asm [OPTION]\n");
    printf("Run the femto assembler with INFILE as source input, OUTFILE as binary output, configure with OPTION.\n\n");
    printf("--help             : display this help & exit\n");
    printf(" -h | -?\n");
    printf("--version          : output version information and exit\n");
    printf(" -v\n");
    printf("--file [INFILE]    : specify the source file to be assemble\n");
    printf(" -f\n");
    printf("--output [OUTFILE] : specify the output file\n");
    printf(" -o\n");
}

void cmd_version(void)
{
    printf("asm %s | Copyright (C) 2021 Semperfis\n", (char *)ASM_VERSION);
    printf("This program comes with ABSOLUTELY NO WARRANTY;\n");
    printf("License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>;\n");
    printf("This is free software, and you are welcome to redistribute it under certain conditions;\n");
}


/*** PROGRAM ENTRY POINT ***/
int main(int argc, char *argv[])
{
    char *src_fname  = NULL;
    char *dst_fname  = NULL;
    FILE *src_file   = NULL;
    FILE *dst_file   = NULL;

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
        else if (strcmp(argv[i], "--output") == 0 || strcmp(argv[i], "-o") == 0)
        {
            i++;
            dst_fname = argv[i];
        }
    }

    /*** FILE HANDLING (OPENING) */
    src_file = fopen((const char *)src_fname, "r");
    if (src_file == NULL)
    {
        printf("ERROR : CAN'T OPEN \"%s\" !!!\n", src_fname);
        return -1;
    }

    dst_file = fopen((const char *)dst_fname, "w+b");
    if (dst_file == NULL)
    {
        printf("ERROR : CAN'T OPEN \"%s\" !!!\n", dst_fname);
        fclose(src_file);
        return -1;
    }

    // DEBUG:
    printf("Succesfully open \"%s\" (source) & \"%s\" (destination)\n", src_fname, dst_fname);


    /*** FILE HANDLING (CLOSING) & PROGRAM EXIT */
    fclose(src_file);
    fclose(dst_file);
    return 0;
}