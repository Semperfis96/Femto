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

#define FEMTO_VERSION "1.0"
#define ADRM_IMM false
#define ADRM_REG true
#define CFLAG ((flags >>  1) & 0x1)
#define ZFLAG  (flags & 0x1)
#define NFLAG ((flags >>  2) & 0x1)


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
    JZ  = 0x8, /* JUMP IF ZERO; SAME AS JE "x == y"*/
    JN  = 0x9, /* JUMP IF NEGATIVE "x < 0" */
    JC  = 0xA, /* JUMP IF CARRY; SAME AS JB (JUMP IF BELOW) "x < y" */
    JNC = 0xB, /* JUMP IF NOT CARRY; SAME AS JAE (JUMP IF ABOVE OR EQUAL) " x >= y"*/
    JBE = 0xC, /* JUMP IF BELOW OR EQUAL (CARRY OR ZERO) "x <= y */
    JA  = 0xD, /* JUMP IF ABOVE (!CARRY OR !ZERO) "x > y" */
    JMP = 0xE
} inst_t;


void cmd_help(void)
{
    printf("Usage: femto [OPTION]\n");
    printf("Run the computer emulator FEMTO with FILE as input binary, configure with OPTION.\n\n");
    printf("--help        : display this help & exit\n");
    printf(" -h | -?\n");
    printf("--version     : output version information and exit\n");
    printf(" -v\n");
    printf("--file [FILE] : specify the binary file to be load in RAM\n");
    printf(" -f\n");
}

void cmd_version(void)
{
    printf("femto %s | Copyright (C) 2021 Semperfis\n", (char *)FEMTO_VERSION);
    printf("This program comes with ABSOLUTELY NO WARRANTY;\n");
    printf("License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>;\n");
    printf("This is free software, and you are welcome to redistribute it under certain conditions;\n");
}

int rom_load(const char *rom_file, uint8_t *ram)
{
    FILE *rom  = NULL;
    long  size = 0L;

    rom = fopen(rom_file, "rb");
    if (rom == NULL)
    {
        printf("ERROR (rom_load): CAN'T OPEN FILE \"%s\" !!!\n", rom_file);
        return -1;
    }

    /* GET ROM SIZE */
    fseek(rom, 0L, SEEK_END);
    size = ftell(rom);
    fseek(rom, 0L, SEEK_SET);

    /* READ ROM TO RAM AT 0x000 */
    if (fread((void *)ram, sizeof(uint8_t), (size_t)size, rom) != (size_t)size)
    {
        printf("ERROR (rom_load): CAN'T READ PROPERLY THE FILE \"%s\" !!!\n", rom_file);
        fclose(rom);
        return -1;
    }

    fclose(rom);
    return 0;
}


uint8_t test_update_flags(int testing)
{
    /* reset the flags */
    uint8_t flags = 0;

    /* test & set in consequence */
    if (testing == 0)
    {
        /* set the zero flag */
        flags = 0x1;
    }
    else if (testing < 0)
    {
        /* set the negative flag */
        flags = 0x4;
    }
    else if (testing > 0xFF)
    {
        /* set the carry flags */
        flags = 0x2;
    }

    return flags;
}


void print_flags(uint8_t flags)
{
    printf("FLAGS: N : %01X; C : %01X; Z : %01X\n", NFLAG, CFLAG, ZFLAG);
}


/*** PROGRAM ENTRY POINT ***/
int main(int argc, char *argv[])
{
    bool      halt  = false;
    uint8_t  *ram   = NULL;      /* VIRTUAL COMPUTER RAM, 4KBs (0x000 - 0xFFF) */
    uint16_t  pc    =  0x0;      /* PROGRAM COUNTER (12BITS) */
    uint8_t   r[4]  = {0x0};     /* GP REGISTERS (R0, R1, R2 AND R3) */
    uint8_t   flags =  0x0;      /* FLAGS REGISTER */
    uint8_t   f[3]  = {0x0};     /* ARRAY OF BINARY FETCH INSTRUCTION (3 BYTES LONG) */
    uint8_t   inst  = 0;         /* INSTRUCTION CODE */
    bool      adrm  = ADRM_IMM;  /* ADDRESSING MODE */
    uint8_t   data  = 0;         /* 8BITS DATA */
    uint16_t  addr  = 0;         /* 12BITS ADDRESS */
    uint8_t   dreg  = 0;         /* DESTINATION REGISTER */
    uint8_t   sreg  = 0;         /* SOURCE REGISTER */
    int       temp  = 0;
    char     *rom   = NULL;

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
            rom = argv[i];
        }
    }


    /*** EMULATION SETUP ***/
    /*   RAM ALLOCATION    */
    ram = malloc(4 * 1024 * sizeof(uint8_t));
    if (ram == NULL)
    {
        printf("ERROR (main): CAN'T ALLOCATE VIRTUAL RAM !!!\n");
        exit(-1);
    }

    /* ROM LOADING (BINARY FILE) INTO RAM */
    if (rom_load((const char *)rom, ram) != 0)
    {
        free(ram);
        return -1;
    }


    /*** EMULATION LOOP ***/
    while (!halt)
    {
        /* FETCH INSTRUCTION FROM RAM */
        printf("[0x%03X] ", pc);
        f[0] = ram[(pc++ % 0xFFF)];
        f[1] = ram[(pc++ % 0xFFF)];
        f[2] = ram[(pc++ % 0xFFF)];

        /* DECODE INSTRUCTION */
        inst =   f[0] & 0x7F;
        adrm =   f[0] & 0x80;
        dreg =  (f[1] >> 6) & 0x03;
        sreg =  (f[1] >> 4) & 0x03;
        data =   f[2];
        addr = ((f[1] & 0x0F) << 8) | f[2];

        /* EXECUTE INSTRUCTION */
        switch((inst_t)inst)
        {
            case HLT:
                printf("HLT INSTRUCTION AT 0x%03X\n", (pc - 3) % 0xFFF);
                halt = true;
                break;
            
            case LDR:
                /* LDR DREG, SREG | IMM */
                if (adrm == ADRM_IMM)
                {
                    r[dreg] = data;
                    printf("LDR: R%d = 0x%02X\n", dreg, r[dreg]);
                }
                else if (adrm == ADRM_REG)
                {
                    r[dreg] = r[sreg];
                    printf("LDR: R%d = R%d (0x%02X)\n", dreg, sreg, r[sreg]);
                }
                break;
            
            case LDM:
                /* LDR DREG, SREG | IMM */
                if (adrm == ADRM_IMM)
                {
                    r[dreg] = ram[addr];
                    printf("LDM: R%d = 0x%02X (RAM[0x%03X])\n", dreg, r[dreg], addr);
                }
                else if (adrm == ADRM_REG)
                {
                    r[dreg] = ram[r[sreg]];
                    printf("LDM: R%d = 0x%02X (RAM[R%d] (0x%02X))\n", dreg, r[dreg], sreg, r[sreg]);
                }
                break;
            
            case STI:
                /* STI REG, IMM */
                if (adrm == ADRM_IMM)
                {
                    ram[r[dreg]] = data;
                    printf("STI: RAM[R%d (0x%02X)] = 0x%02X\n", dreg, r[dreg], ram[r[dreg]]);
                }
                else
                {
                    printf("ILLEGAL ADDRESSING MODES (REGISTER) FOR STI AT 0x%03X\n", (pc - 3) % 0xFFF);
                    halt = true;
                }
                break;
            
            case STR:
                /* STM IMM | REG, REG */
                if (adrm == ADRM_IMM)
                {
                    ram[addr] = r[sreg];
                    printf("STR: RAM[0x%03X] = 0x%02X (R%d (0x%02X))\n", addr, ram[addr], sreg, r[sreg]);
                }
                else if (adrm == ADRM_REG)
                {
                    ram[r[dreg]] = r[sreg];
                    printf("STR: RAM[R%d (0x%02X)] = 0x%02X (R%d (0x%02X))\n", dreg, r[dreg], ram[r[dreg]], sreg, r[sreg]);
                }
                break;
            
            case ADD:
                /* ADD REG, REG */
                temp = (int)r[dreg] + (int)r[sreg];
                flags = test_update_flags(temp);
                temp = r[dreg];
                r[dreg] += r[sreg];
                printf("ADD: R%d (0x%02X) = R%d (0x%02X) + R%d (0x%02X)\n", dreg, r[dreg], dreg, r[dreg] - r[sreg], sreg, r[sreg]);
                print_flags(flags);
                break;
            
            case SUB:
                /* SUB REG, REG */
                temp = (int)r[dreg] - (int)r[sreg];
                flags = test_update_flags(temp);
                temp = r[dreg];
                r[dreg] -= r[sreg];
                printf("SUB: R%d (0x%02X) = R%d (0x%02X) - R%d (0x%02X)\n", dreg, r[dreg], dreg, temp, sreg, r[sreg]);
                print_flags(flags);
                break;
            
            case CMP:
                /* CMP REG, REG */
                temp = r[dreg] - r[sreg];
                flags = test_update_flags(temp);
                printf("CMP: R%d (0x%02X), R%d (0x%02X)\n", dreg, r[dreg], sreg, r[sreg]);
                print_flags(flags);
                break;
            
            case JMP:
                /* JMP IMM */
                pc = addr;
                printf("JMP: 0x%03X (PC = 0x%03X)\n", addr, pc);
                break;
            
            case JZ:
                /* JZ/JE IMM */
                if (ZFLAG == 1)
                {
                    pc = addr;
                    printf("JZ/JE: 0x%03X (PC = 0x%03X)\n", addr, pc);
                }
                else
                {
                    printf("NOT TAKEN JZ/JE: 0x%03X (PC = 0x%03X)\n", addr, pc);
                }
                break;
            
            case JN:
                /* JN IMM */
                if (NFLAG == 1)
                {
                    pc = addr;
                    printf("JN: 0x%03X (PC = 0x%03X)\n", addr, pc);
                }
                else
                {
                    printf("NOT TAKEN JN: 0x%03X (PC = 0x%03X)\n", addr, pc);
                }
                break;

            default:
                printf("ILLEGAL OPCODE 0x%02X AT 0x%03X\n", inst, (pc - 3) % 0xFFF);
                halt = true;
                break;
        }
        temp = 0;
    }

    /*** EMULATION END ***/
    free(ram);
    return 0;
}