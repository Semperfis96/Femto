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

#ifndef COMMON_H_
#define COMMON_H_

#include <stdint.h>
#include <stdbool.h>

#define FEMTO_VERSION "1.0.0"
#define ASM_VERSION   "1.0.0"
#define DISM_VERSION  "1.0.0"
#define TEST_VERSION  "1.0.0"

/* STACK RELATED STUFF */
#define STACK_BASE  0xF00

/* FLAGS: XXXX XNCZ */
#define CFLAG ((emu->flags >> 1) & 0x1)
#define ZFLAG  (emu->flags & 0x1)
#define NFLAG ((emu->flags >> 2) & 0x1)
#define IFLAG ((emu->flags >> 3) & 0x1)

#define ADRM_IMM  false
#define ADRM_REG  true

#endif