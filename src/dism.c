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

#ifdef _EMU_
    #undef _EMU_
#endif

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


void disasm_inst(uint8_t byte, char *result)
{
    int i = 0;

    /* FIND THE INSTRUCTION IN THE TRANSLATION TABLE */
    while (inst_trans_table[i].value != byte)
    {
        i++;
    }

    strncat(result, inst_trans_table[i].str, DISM_BUFFER);
}


void disasm_reg(uint8_t reg, char *result)
{
    int i = 0;

    /* FIND THE REGISTER IN THE TRANSLATION TABLE */
    while (reg_trans_table[i].value != reg)
    {
        i++;
    }

    strncat(result, reg_trans_table[i].str, DISM_BUFFER);
}


void disasm_dest(uint8_t finst, uint8_t dst, uint8_t data, char *result)
{
    int  temp   = 0;
    char tmp_str[4];
    char tmp_result[8] = "0x";
    uint8_t inst = finst & 0x7F;

    /* NOTHING TO DO, NO FIELD */
    if (inst_trans_table[inst].dst == NONE)
    {
        return;
    }
    /* REGISTER DEST FIELD */
    else if(inst_trans_table[inst].dst == REG)
    {
        temp = (dst & 0xC0) >> 6;
        disasm_reg((uint8_t)temp, result);
        return;
    }
    /* IMMEDIATE DEST FIELD */
    else if(inst_trans_table[inst].dst == IMM)
    {
        /* VERIFY IF THIS IS AN ADDRESS FIELD OR NOT */
        if (inst_trans_table[inst].is_addr)
        {
            snprintf(tmp_str, 4, "%03X", ((dst & 0x0F) << 8) | data);
            strncat(tmp_result, (const char *)tmp_str, 8);
            strncat(result, (const char *)tmp_result, DISM_BUFFER);
        }
        else
        {
            snprintf(tmp_str, 4, "%02X", data);
            strncat(tmp_result, (const char *)tmp_str, 8);
            strncat(result, (const char *)tmp_result, DISM_BUFFER);
        }
    }
    /* REGISTER OR IMMEDIATE DEST FIELD */
    else if(inst_trans_table[inst].dst == BOTH)
    {
        /* VERIFY THE ADDRESSING MODE OF THE INSTRUCTION */
        if (((finst & 0x80) >> 7) == ADRM_IMM)
        {
            /* IMMEDIATE ADDRESSING MODE */
            /* VERIFY IF THIS IS AN ADDRESS FIELD OR NOT */
            if (inst_trans_table[inst].is_addr)
            {
                snprintf(tmp_str, 4, "%03X", ((dst & 0x0F) << 8) | data);
                strncat(tmp_result, (const char *)tmp_str, 8);
                strncat(result, (const char *)tmp_result, DISM_BUFFER);
            }
            else
            {
                snprintf(tmp_str, 4, "%02X", data);
                strncat(tmp_result, (const char *)tmp_str, 8);
                strncat(result, (const char *)tmp_result, DISM_BUFFER);
            }
        }
        else
        {
            /* REGISTER ADDRESSING MODE */
            temp = (dst & 0xC0) >> 6;
            disasm_reg((uint8_t)temp, result);
            return;
        }
    }
}


void disasm_src(uint8_t finst, uint8_t dst, uint8_t data, char *result)
{
    int  temp   = 0;
    char tmp_str[4];
    char tmp_result[8] = "0x";
    uint8_t inst = finst & 0x7F;

    /* NOTHING TO DO, NO FIELD */
    if (inst_trans_table[inst].src == NONE)
    {
        return;
    }
    /* REGISTER SRC FIELD */
    else if(inst_trans_table[inst].src == REG)
    {
        temp = (dst & 0xC0) >> 6;
        disasm_reg((uint8_t)temp, result);
        return;
    }
    /* IMMEDIATE SRC FIELD */
    else if(inst_trans_table[inst].src == IMM)
    {
        /* VERIFY IF THIS IS AN ADDRESS FIELD OR NOT */
        if (inst_trans_table[inst].is_addr)
        {
            snprintf(tmp_str, 4, "%03X", ((dst & 0x0F) << 8) | data);
            strncat(tmp_result, (const char *)tmp_str, 8);
            strncat(result, (const char *)tmp_result, DISM_BUFFER);
        }
        else
        {
            snprintf(tmp_str, 4, "%02X", data);
            strncat(tmp_result, (const char *)tmp_str, 8);
            strncat(result, (const char *)tmp_result, DISM_BUFFER);
        }
    }
    /* REGISTER OR IMMEDIATE SRC FIELD */
    else if(inst_trans_table[inst].src == BOTH)
    {
        /* VERIFY THE ADDRESSING MODE OF THE INSTRUCTION */
        if (((finst & 0x80) >> 7) == ADRM_IMM)
        {
            /* IMMEDIATE ADDRESSING MODE */
            /* VERIFY IF THIS IS AN ADDRESS FIELD OR NOT */
            if (inst_trans_table[inst].is_addr)
            {
                snprintf(tmp_str, 4, "%03X", ((dst & 0x0F) << 8) | data);
                strncat(tmp_result, (const char *)tmp_str, 8);
                strncat(result, (const char *)tmp_result, DISM_BUFFER);
            }
            else
            {
                snprintf(tmp_str, 4, "%02X", data);
                strncat(tmp_result, (const char *)tmp_str, 8);
                strncat(result, (const char *)tmp_result, DISM_BUFFER);
            }
        }
        else
        {
            /* REGISTER ADDRESSING MODE */
            temp = (dst & 0xC0) >> 6;
            disasm_reg((uint8_t)temp, result);
            return;
        }
    }
}


void disasm(uint8_t *buffer, uint16_t pc, char *result)
{
    uint8_t f[3] = {0};
    uint8_t inst = 0;

    /* RESET RESULT STRING */
    memset((void *)result, 0, DISM_BUFFER);

    /* EXTRACT FIELD FROM RAW BINARY */
    f[0] = buffer[pc];
    f[1] = buffer[(pc + 1)];
    f[2] = buffer[(pc + 2)];
    inst = (f[0] & 0x7F);

    /* DISASSEMBLE INSTRUCTION */
    disasm_inst(inst, result);
    strncat(result, " ", DISM_BUFFER);

    /* DISASSEMBLE IF NECESSARY DESTINATION FIELD & IF NECESSARY SRC FIELD */
    if (inst_trans_table[inst].dst != NONE)
    {
        disasm_dest(f[0], f[1], f[2], result);
        if (inst_trans_table[inst].src != NONE)
        {
            strncat(result, ", ", DISM_BUFFER);
            disasm_src(f[0],  f[2], f[2], result);
        }
    }
}


/*** PROGRAM ENTRY POINT ***/
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


    /*** ALLOCATE MEMORY FOR DISASSEMBLER RESULT STRING ***/
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

    /* ALLOC MEMORY FOR THE FILE BUFFER */
    src_bin = malloc((int)src_size * sizeof(uint8_t));
    if (src_bin == NULL)
    {
        printf("ERROR (main) : CAN'T ALLOC MEM FOR BUFFER !!!\n");
        fclose(src_file);
        exit(-1);
    }

    /* READ BINARY FILE & CLOSE IT */
    if (fread((void *)src_bin, sizeof(uint8_t), (size_t)src_size, src_file) != (size_t)src_size)
    {
        printf("ERROR (main): CAN'T READ PROPERLY THE FILE \"%s\" !!!\n", src_fname);
        fclose(src_file);
        exit(-1);
    }

    fclose(src_file);


    /*** DISASSEMBLING ***/
    /* FORMAT: "ADDRESS : BINARY_INSTRUCTION    DIASSEMBLING_RESULT" */
    for (int i = 0; i < src_size; i += 3)
    {
        disasm(src_bin, i, result);
        printf("%03X : %02X%02X%02X\t%s\n", i, src_bin[i], src_bin[i+1], src_bin[i+2], result);
    }

    return 0;
}
/*** CODE ENDING ***/