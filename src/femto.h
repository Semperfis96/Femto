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

#ifndef FEMTO_H_
#define FEMTO_H_

#include <stdint.h>
#include <stdbool.h>


typedef struct FemtoEmu
{
    uint16_t  pc;      /* PROGRAM COUNTER (12BITS) */
    uint8_t   r[4];    /* GP REGISTERS (R0, R1, R2 AND R3) */
    uint8_t   sp;      /* STACK POINTER */
    uint8_t   flags;   /* FLAGS REGISTER */
    uint8_t   f[3];    /* ARRAY OF BINARY FETCH INSTRUCTION (3 BYTES LONG) */
    uint8_t   inst;    /* INSTRUCTION CODE */
    uint8_t  *ram;     /* VIRTUAL COMPUTER RAM, 4KBs (0x000 - 0xFFF) */
    bool      adrm;    /* ADDRESSING MODE */
    bool      halt;    /* CPU IS HALT OR NOT */
    uint8_t   data;    /* 8BITS DATA */
    uint16_t  addr;    /* 12BITS ADDRESS */
    uint8_t   dreg;    /* DESTINATION REGISTER */
    uint8_t   sreg;     /* SOURCE REGISTER */
    int       temp;
} FemtoEmu_t;


FemtoEmu_t * EmuInit(const char *rom_file, bool verbose);
void         EmuQuit(FemtoEmu_t *emu);
void         EmuLoop(FemtoEmu_t *emu, bool verbose);

#endif