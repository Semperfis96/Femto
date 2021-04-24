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
 * - MIII IIII   RRRR xxxx   DDDD DDDD
 * - MIII IIII   RRRR AAAA   AAAA AAAA
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include "../common.h"
#include "asm.h"


#define get_token strtok(NULL, " ,\n")


/*** CODE BEGINNING ***/
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


bool is_reg(char *token, int *range)
{
    int i = 0;

    /* FIND THE REGISTER IN TRANSLATION TABLE */
    while ((strcmp((const char *)reg_trans_table[i].str, (const char *)token) != 0) && i < 4)
    {
        i++;
    }

    /* THERE IS ONLY 4 REGISTERS THAT PROGRAMMERS CAN DIRECTLY ADDRESSING */
    if (i < 4)
    {
        *range = i;
        return true;
    }
    else
    {
        return false;
    }
    
}


/* RETURN "TRUE" ON ILLEGAL THING */
bool dst_assembler(char *token, uint8_t inst, uint8_t *dreg, uint16_t *addr, uint8_t *data, bool *adrm)
{
    int temp = 0;
    int j = inst; // HACK: TO BE FIX

    /* VERIFY IF WE HAVE A DESTINATION FIELD */
    if (inst_trans_table[j].dst != NONE)
    {
        token = get_token;

        /* DST FIELD ONLY SUPPORT REGISTER, SO VERIFY IF THERE IS A REGISTER AND ADD IT TO INSTRUCTION */
        if (inst_trans_table[j].dst == REG)
        {
            if (is_reg(token, &temp))
            {
                *dreg = reg_trans_table[temp].value;
                printf("DST FIELD REGISTER R%d\n", *dreg);
            }
            else
            {
                printf("(dst_assembler) ERROR: ILLEGAL ADDRESSING MODE \"%s\"\n", token);
                return true;
            }
        }
        /* DST FIELD ONLY SUPPORT IMMEDIATE, SO VERIFY IF THERE IS AN IMMEDIATE AND ADD IT TO INSTRUCTION */
        else if (inst_trans_table[j].dst == IMM)
        {
            if (!is_reg(token, &temp))
            {
                /* INSTRUCTION NEED DATA OR ADDRESS */
                if (inst_trans_table[j].is_addr == true)
                {
                    temp = (int)strtol((const char *)token, NULL, 0);

                    /* VERIFY THE SIZE OF THE ADDRESS NO MORE THAN 3 BYTES */
                    if (temp < 0x1000)
                    {
                        *addr = (uint16_t)temp;
                        printf("DST FIELD ADDR 0x%03X\n", *addr);
                    }
                    else
                    {
                        printf("(dst_assembler) ADDR FIELD WITH TOO BIG VALUE : 0x%X\n", temp);
                        return true;
                    }
                }
                else
                {
                    temp = (int)strtol((const char *)token, NULL, 0);

                    /* VERIFY THE SIZE OF THE DATA NO MORE THAN 2 BYTES */
                    if (temp < 0x100)
                    {
                        *data = (uint8_t)temp;
                        printf("DST FIELD DATA 0x%02X\n", *data);
                    }
                    else
                    {
                        printf("(dst_assembler) DATA FIELD WITH TOO BIG VALUE : 0x%X\n", temp);
                        return true;
                    }
                }
            }
            else
            {
                printf("(dst_assembler) ERROR: ILLEGAL ADDRESSING MODE \"%s\"\n", token);
                return true;
            }
        }
        /* DST FIELD SUPPORT ALL, SO VERIFY IF THERE IS AN IMMEDIATE OR A REGISTER AND ADD WHATEVER TO INSTRUCTION */
        else if (inst_trans_table[j].dst == BOTH)
        {
            /* SET ADRM FIELD IS ONLY NEEDED WHEN INSTRUTION SUPPORT BOTH ADDRESSING MODES */
            if (is_reg(token, &temp))
            {
                *dreg = reg_trans_table[temp].value;
                *adrm = ADRM_REG;
                printf("DST FIELD REGISTER R%d\n", *dreg);
            }
            else
            {
                adrm = ADRM_IMM;
                if (inst_trans_table[j].is_addr == true)
                {
                    temp = (int)strtol((const char *)token, NULL, 0);

                    /* VERIFY THE SIZE OF THE ADDRESS NO MORE THAN 3 BYTES */
                    if (temp < 0x1000)
                    {
                        *addr = (uint16_t)temp;
                        printf("DST FIELD ADDR 0x%03X\n", *addr);
                    }
                    else
                    {
                        printf("(dst_assembler) ADDR FIELD WITH TOO BIG VALUE : 0x%X\n", temp);
                        return true;
                    }
                }
                else
                {
                    temp = (int)strtol((const char *)token, NULL, 0);

                    /* VERIFY THE SIZE OF THE DATA NO MORE THAN 2 BYTES */
                    if (temp < 0x100)
                    {
                        *data = (uint8_t)temp;
                        printf("DST FIELD DATA 0x%02X\n", *data);
                    }
                    else
                    {
                        printf("(dst_assembler) DATA FIELD WITH TOO BIG VALUE : 0x%X\n", temp);
                        return true;
                    }
                }
            }
        }
    }

    return false;
}


/* RETURN "TRUE" ON ILLEGAL THING */
bool src_assembler(char *token, uint8_t inst, uint8_t *sreg, uint16_t *addr, uint8_t *data, bool *adrm)
{
    int temp = 0;
    int j = inst; // HACK: TO BE FIX

    /* VERIFY IF WE HAVE A SOURCE FIELD */
    if (inst_trans_table[j].src != NONE)
    {
        token = get_token;

        /* SRC FIELD ONLY SUPPORT REGISTER, SO VERIFY IF THERE IS A REGISTER AND ADD IT TO INSTRUCTION */
        if (inst_trans_table[j].src == REG)
        {
            if (is_reg(token, &temp))
            {
                *sreg = reg_trans_table[temp].value;
                printf("SRC FIELD REGISTER R%d\n", *sreg);
            }
            else
            {
                printf("(src_assembler) ERROR: ILLEGAL ADDRESSING MODE \"%s\"\n", token);
                return true;
            }
        }
        /* SRC FIELD ONLY SUPPORT IMMEDIATE, SO VERIFY IF THERE IS AN IMMEDIATE AND ADD IT TO INSTRUCTION */
        else if (inst_trans_table[j].src == IMM)
        {
            if (!is_reg(token, &temp))
            {
                /* INSTRUCTION NEED DATA OR ADDRESS */
                if (inst_trans_table[j].is_addr == true)
                {
                    temp = (int)strtol((const char *)token, NULL, 0);

                    /* VERIFY THE SIZE OF THE ADDRESS NO MORE THAN 3 BYTES */
                    if (temp < 0x1000)
                    {
                        *addr = (uint16_t)temp;
                        printf("SRC FIELD ADDR 0x%03X\n", *addr);
                    }
                    else
                    {
                        printf("(src_assembler) ADDR FIELD WITH TOO BIG VALUE : 0x%X\n", temp);
                        return true;
                    }
                }
                else
                {
                    temp = (int)strtol((const char *)token, NULL, 0);

                    /* VERIFY THE SIZE OF THE DATA NO MORE THAN 2 BYTES */
                    if (temp < 0x100)
                    {
                        *data = (uint8_t)temp;
                        printf("SRC FIELD DATA 0x%02X\n", *data);
                    }
                    else
                    {
                        printf("(src_assembler) DATA FIELD WITH TOO BIG VALUE : 0x%X\n", temp);
                        return true;
                    }
                }
            }
            else
            {
                printf("(src_assembler) ERROR: ILLEGAL ADDRESSING MODE \"%s\"\n", token);
                return true;
            }
        }
        /* SRC FIELD SUPPORT ALL, SO VERIFY IF THERE IS AN IMMEDIATE OR A REGISTER AND ADD WHATEVER TO INSTRUCTION */
        else if (inst_trans_table[j].src == BOTH)
        {
            /* SET ADRM FIELD IS ONLY NEEDED WHEN INSTRUTION SUPPORT BOTH ADDRESSING MODES */
            if (is_reg(token, &temp))
            {
                *sreg = reg_trans_table[temp].value;
                *adrm = ADRM_REG;
                printf("SRC FIELD REGISTER R%d\n", *sreg);
            }
            else
            {
                adrm = ADRM_IMM;
                if (inst_trans_table[j].is_addr == true)
                {
                    temp = (int)strtol((const char *)token, NULL, 0);

                    /* VERIFY THE SIZE OF THE ADDRESS NO MORE THAN 3 BYTES */
                    if (temp < 0x1000)
                    {
                        *addr = (uint16_t)temp;
                        printf("SRC FIELD ADDR 0x%03X\n", *addr);
                    }
                    else
                    {
                        printf("(src_assembler) ADDR FIELD WITH TOO BIG VALUE : 0x%X\n", temp);
                        return true;
                    }
                }
                else
                {
                    temp = (int)strtol((const char *)token, NULL, 0);

                    /* VERIFY THE SIZE OF THE DATA NO MORE THAN 2 BYTES */
                    if (temp < 0x100)
                    {
                        *data = (uint8_t)temp;
                        printf("SRC FIELD DATA 0x%02X\n", *data);
                    }
                    else
                    {
                        printf("(src_assembler) DATA FIELD WITH TOO BIG VALUE : 0x%X\n", temp);
                        return true;
                    }
                }
            }
        }
    }

    return false;
}


bool inst_assembler(char *token, uint8_t *inst)
{
    int i = 0;

    /* FIND THE INSTRUCTION IN THE TRANSLATION TABLE */
    while ((strcmp(inst_trans_table[i].str, (const char *)token) != 0) && i < 0xFF)
    {
        i++;
    }

    /* GET THE OPCODE FROM THE VALID INSTRUCTION */
    if (i < 0xFF)
    {
        *inst = inst_trans_table[i].value;
        return false;
    }
    else
    {
        return true;
    }
}


/* TODO: ADD LABEL SUPPORT (ENABLE MULTI-PASS ASSEMBLING), ADD DIRECTVES SUPPORT (ORG, DB, DW, DD, ETC.) */
/*** PROGRAM ENTRY POINT ***/
int main(int argc, char *argv[])
{
    char    *src_fname  = NULL;         /* SOURCE FILE NAME */
    char    *dst_fname  = NULL;         /* DESTINATION FILE NAME */
    char    *token      = NULL;         /* TOKEN ;-) */
    char    *line       = NULL;
    FILE    *src_file   = NULL;         /* SOURCE FILE (ASSEMBLY LANGUAGE) */
    FILE    *dst_file   = NULL;         /* DESTINATION FILE (BINARY) */
    bool     adrm       = ADRM_IMM;     /* ADDRESSING MODE */
    uint8_t  f[3]       = {0x0};        /* ARRAY OF BINARY FETCH INSTRUCTION (3 BYTES LONG) */
    uint8_t  inst       = 0;            /* INSTRUCTION CODE */
    uint8_t  data       = 0;            /* 8BITS DATA */
    uint8_t  dreg       = 0;            /* DESTINATION REGISTER */
    uint8_t  sreg       = 0;            /* SOURCE REGISTER */
    uint16_t addr       = 0;            /* 12BITS ADDRESS */
    int      line_num   = 1;


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


    /* LINE BUFFER */
    line = malloc(256 * sizeof(char));
    if (line == NULL)
    {
        printf("(main) ERROR: CAN'T ALLOCATE MEMORY FOR LINE BUFFER !!!\n");
        return -1;
    }


    /*** FILE HANDLING (OPENING) ***/
    src_file = fopen((const char *)src_fname, "r");
    if (src_file == NULL)
    {
        printf("(main) ERROR : CAN'T OPEN \"%s\" !!!\n", src_fname);
        free(line);
        return -1;
    }

    dst_file = fopen((const char *)dst_fname, "w+b");
    if (dst_file == NULL)
    {
        printf("(main) ERROR : CAN'T OPEN \"%s\" !!!\n", dst_fname);
        fclose(src_file);
        free(line);
        return -1;
    }


    /*** ASSEMBLER ***/
    while (fgets(line, 256, src_file) != NULL)
    {
        /* CONVERT STRING TO UPPER TO SUPPORT LOWER CASE IN SOURCE FILE */
        for (int i = 0; i < 256; i++)
        {
            line[i] = toupper(line[i]);
        }

        /* GET TOKEN */
        token = strtok(line, " ,\n");

        while (token != NULL)
        {
            /* DETECT COMMENTS (START BY #) */
            if (token[0] == '#')
            {
                break;
            }

            /*** INST ASSEMBLING ***/
            if (inst_assembler(token, &inst))
            {
                printf("(main) ILLEGAL INSTRUCTION AT LINE %d : \"%s\"\n", line_num, (const char *)token);
                fclose(dst_file);
                fclose(src_file);
                free(line);
                return -1;
            }

            /*** DST ASSEMBLING ***/
            if (dst_assembler(token, inst, &dreg, &addr, &data, &adrm))
            {
                printf("(main) ILLEGAL DST FIELD AT LINE %d\n", line_num);
                fclose(dst_file);
                fclose(src_file);
                free(line);
                return -1;
            }

            /*** SRC ASSEMBLING ***/
            if (src_assembler(token, inst, &sreg, &addr, &data, &adrm))
            {
                printf("(main) ILLEGAL SRC FIELD AT LINE %d\n", line_num);
                fclose(dst_file);
                fclose(src_file);
                free(line);
                return -1;
            }


            /*** COMBINE ASSEMBLING RESULT & WRITE TO FILE ***/
            f[0] = inst | (adrm << 7);
            f[1] = (dreg << 6) | (sreg << 4) | ((addr & 0xF00) >> 8);
            f[2] = data | (addr & 0x0FF);

            if (fwrite((const void *)f, 1, 3, dst_file) < 3)
            {
                printf("(main) ERROR DURING WRITING IN \"%s\"\n", dst_fname);
            }


            /*** RESET VARIABLES BECAUSE THERE ARE USED PER INSTRUCTION ASSEMBLING ***/
            inst  = 0;
            dreg  = 0;
            sreg  = 0;
            addr  = 0;
            data  = 0;
            adrm  = ADRM_IMM;
            token = get_token;
        }

        line_num++;
    }


    if (!feof(src_file))
    {
       printf("(main) ERROR WHILE READING \"%s\", NOT EOF !!!", src_fname);
       return -1;
    }


    /*** FILE HANDLING (CLOSING) & FREE BUFFER & PROGRAM EXIT ***/
    fclose(dst_file);
    fclose(src_file);
    free(line);
    return 0;
}
/*** CODE ENDING ***/