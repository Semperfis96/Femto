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
 * - STRUCTURE OF FLAGS REGISTER: XXXX INCZ (I : INTERRUPT; N : Negative; C : Carry; Z : Zero)
 * - INSTRUCTION FORMAT: (I: INST; M : ADDRESSING MODES; R : REGISTERS; D : DATA; A : ADDRESS)
 * - MIII IIII   RRRR xxxx   DDDD DDDD
 * - MIII IIII   RRRR AAAA   AAAA AAAA
 */

#ifndef INT_H_
#define INT_H_

#include <stdbool.h>
#include "../cpu/cpu.h"

#define IREQ_VEC        0x000   /* IREQ VECTOR (WHEN IREQ, JUMP TO ADDRESS CONTAIN IN THIS VECTOR) ==> ADDRESS IS STORED BIG-ENDIAN */
#define SYS_VEC         0x002   /* SYS  VECTOR (WHEN SYS INSTRUCTION, JUMP TO ADDRESS IN THIS VECTOR) ==> ADDRESS IS STORED BIG-ENDIAN */
#define GET_ADDR_VEC(v) (RAM[v + 1] << 8) | RAM[v]


#define IREQ(e)               e->ireq = true;
#define RES_IREQ(e)           e->ireq = false;
#define CHK_IREQ(e)          (e->ireq == true)
#define CHK_IRQ_ENABLE(e)   ((e->ireq & 0x8) == 1)
#define ENABLE_IREQ(e)        e->flags |= 1 << 3;
#define DISABLE_IREQ(e)       e->flags &= ~(1 << 3);

void IntReq(FemtoEmu_t *emu);
void SysReq(FemtoEmu_t *emu);

#endif