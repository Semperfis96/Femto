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
#include "cpu/cpu.h"
#include "common.h"


/*** HELPING FUNCTIONS ***/
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


void ResetEmuState(FemtoEmu_t *emu)
{
    emu->pc    = 0x0;       /* PROGRAM COUNTER (12BITS) */
    emu->r[0]  = 0x0;       /* GP REGISTERS (R0, R1, R2 AND R3) */
    emu->r[1]  = 0x0;       /* GP REGISTERS (R0, R1, R2 AND R3) */
    emu->r[2]  = 0x0;       /* GP REGISTERS (R0, R1, R2 AND R3) */
    emu->r[3]  = 0x0;       /* GP REGISTERS (R0, R1, R2 AND R3) */
    emu->f[0]  = 0x0;
    emu->f[1]  = 0x0;
    emu->f[2]  = 0x0;
    emu->sp    = 0x0;
    emu->inst  = 0x0;
    emu->flags = 0x0;       /* FLAGS REGISTER */
    emu->adrm  = ADRM_IMM;  /* ADDRESSING MODE */
    emu->halt  = false;     /* CPU IS HALT OR NOT */
    emu->data  = 0;         /* 8BITS DATA */
    emu->addr  = 0;         /* 12BITS ADDRESS */
    emu->dreg  = 0;         /* DESTINATION REGISTER */
    emu->sreg  = 0;         /* SOURCE REGISTER */
    emu->temp  = 0;
}
/*** END OF HELPING FUNCTIONS ***/


FemtoEmu_t * EmuInit(const char *rom_file, bool verbose)
{
    FemtoEmu_t *temp = NULL;


    /* EMULATION STATE ALLOCATION */
    temp = malloc(sizeof(FemtoEmu_t));
    if (temp == NULL)
    {
        printf("ERROR (EmuInit): CAN'T ALLOCATE EMULATION STATE!!!\n");
        exit(-1);
    }
    if (verbose == true) printf("FEMTO: EMULATION STATE IS ALLOCATE\n");
    ResetEmuState(temp);


    /* RAM ALLOCATION */
    temp->ram = malloc(4 * 1024 * sizeof(uint8_t));
    if (temp->ram == NULL)
    {
        printf("ERROR (EmuInit): CAN'T ALLOCATE VIRTUAL RAM !!!\n");
        free(temp);
        exit(-1);
    }
    if (verbose == true) printf("FEMTO: VIRTUAL RAM IS ALLOCATE\n");


    /* ROM LOADING (BINARY FILE) INTO RAM */
    if (rom_load(rom_file, temp->ram) != 0)
    {
        free(temp->ram);
        free(temp);
        exit(-1);
    }
    if (verbose == true) printf("FEMTO: ROM IS LOAD IN VIRTUAL RAM\n");

    return temp;
}


void EmuLoop(FemtoEmu_t *emu, bool verbose)
{
    /*** EMULATION LOOP ***/
    printf("FEMTO: STARTING EMULATION\n");
    while (!emu->halt)
    {
        CpuExecInst(emu, verbose);
    }
}


void EmuQuit(FemtoEmu_t *emu)
{
    printf("FEMTO: HALTING EMULATION\n");
    free(emu->ram);
    free(emu);
}