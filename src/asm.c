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

#define ASM_VERSION "1.0.0"
#define ADRM_IMM  false
#define ADRM_REG  true
#define INST_NUM  0xF
#define get_token strtok(NULL, " ,\n")


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

typedef enum field_adrm
{
    NONE,   /* FIELD NOT USE, SO NOT AVAILABLE */
    REG,    /* FIELD SUPPORT REGISTER ADDRESSING MODE */
    IMM,    /* FIELD SUPPORT IMMEDIATE ADDRESSING MODE */
    BOTH    /* FIELD SUPPORT BOTH ADDRESSING MODES */
} field_adrm_t;

typedef struct trans
{
    const char   str[4];     /* THE MNEMONIC STRING (EX: LDR; ADD; etc.) */
    uint8_t      value;      /* THE TRANSLATED OPCODE OF THE MNEMONIC */
    field_adrm_t dst;        /* INSTRUCTION DESTINATION FIELD */
    field_adrm_t src;        /* INSTRUCTION SOURCE FIELD */
} trans_t;

const trans_t translate[] =
{
    {"HLT", HLT, NONE, NONE},
    {"LDR", LDR, REG,  BOTH},
    {"LDM", LDM, REG,  BOTH},
    {"STI", STI, REG,  IMM},
    {"STR", STR, BOTH, REG},
    {"ADD", ADD, REG,  REG},
    {"SUB", SUB, REG,  REG},
    {"CMP", CMP, REG,  REG},
    {"JMP", JMP, IMM,  NONE},
    {"JZ",  JZ , IMM,  NONE},
    {"JN",  JN , IMM,  NONE},
    {"JC",  JC , IMM,  NONE},
    {"JNC", JNC, IMM,  NONE},
    {"JBE", JBE, IMM,  NONE},
    {"JA",  JA , IMM,  NONE}
};

const trans_t registers[] =
{
    {"R0", 0x0, NONE, NONE},
    {"R1", 0x1, NONE, NONE},
    {"R2", 0x2, NONE, NONE},
    {"R3", 0x3, NONE, NONE}
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


bool is_token_register(char *token, int *range)
{
    int i = 0;

    while ((strcmp((const char *)registers[i].str, (const char *)token) != 0))
    {
        if (i < 4)
        {
            i++;
        }
        else
        {
            return false;
        }
    }

    *range = i;
    return true;
}


/*** PROGRAM ENTRY POINT ***/
int main(int argc, char *argv[])
{
    char    *src_fname  = NULL;
    char    *dst_fname  = NULL;
    char    *src_buffer = NULL;
    char    *token      = NULL;
    FILE    *src_file   = NULL;
    FILE    *dst_file   = NULL;
    long     fsize      = 0L;
    int      i          = 0;
    int      j          = 0;
    int      temp       = 0;
    bool     illegal    = false;
    bool     adrm       = ADRM_IMM;  /* ADDRESSING MODE */
    uint8_t  f[3]       = {0x0};     /* ARRAY OF BINARY FETCH INSTRUCTION (3 BYTES LONG) */
    uint8_t  inst       = 0;         /* INSTRUCTION CODE */
    uint8_t  data       = 0;         /* 8BITS DATA */
    uint8_t  dreg       = 0;         /* DESTINATION REGISTER */
    uint8_t  sreg       = 0;         /* SOURCE REGISTER */
    uint16_t addr       = 0;         /* 12BITS ADDRESS */


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


    /*** FILE HANDLING (OPENING) ***/
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


    /*** FILE HANDLING (LOADING SRC FILE IN RAM) ***/
    /* GET FILE SIZE */
    fseek(src_file, 0L, SEEK_END);
    fsize = ftell(src_file);
    fseek(src_file, 0L, SEEK_SET);

    src_buffer = malloc(fsize * sizeof(char));
    if (src_buffer == NULL)
    {
        printf("ERROR: CAN'T ALLOCATE MEMORY FOR SOURCE FILE BUFFER !!!\n");
        fclose(src_file);
        fclose(dst_file);
        return -1;
    }

    if (fread((void *)src_buffer, sizeof(char), (size_t)fsize, src_file) != (size_t)fsize)
    {
        printf("ERROR: CAN'T READ PROPERLY THE FILE \"%s\" !!!\n", src_fname);
        fclose(src_file);
        fclose(dst_file);
        free(src_buffer);
        return -1;
    }


    /*** ASSEMBLER ***/
    token = strtok(src_buffer, " ,\n");
    while (token != NULL)
    {
        /* FIND THE INSTRUCTION IN THE TRANSLATION TABLE */
        while ((strcmp(translate[j].str, (const char *)token) != 0))
        {
            if (j < INST_NUM)
            {
                j++;
            }
            else
            {
                printf("ILLEGAL INSTRUCTION \"%s\"\n", token);
                illegal = true;
                break;
            }
        }

        /* GET THE OPCODE FROM THE VALID INSTRUCTION */
        if (!illegal)
        {
            inst = translate[j].value;
        }

        /* INSTRUCTION NEED DST FIELD, EITHER REGISTER OR IMMEDIATE */
        if (translate[j].need_dst)
        {
            token = get_token;
            if (is_token_register(token, &temp))
            {
                dreg = registers[temp].value;
            }
        }

        /* INSTRUCTION NEED SRC FIELD, EITHER REGISTER OR IMMEDIATE */
        if (translate[j].need_src)
        {
            token = get_token;
            if (is_token_register(token, &temp))
            {
                sreg = registers[temp].value;
            }
        }

        //DEBUG
        printf("INST = 0x%02X; DREG = 0x%01X; SREG = 0x%01X\n", inst, dreg, sreg);

        /* reset variable use per instruction assembling */
        j       = 0;
        illegal = false;
        temp = 0;
        inst = 0;
        dreg = 0;
        sreg = 0;

        token   = get_token;
        i++;
    }


    /*** FILE HANDLING (CLOSING) & PROGRAM EXIT ***/
    fclose(src_file);
    fclose(dst_file);
    free(src_buffer);
    return 0;
}