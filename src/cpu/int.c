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

#include <stdio.h>
#include "../femto.h"
#include "cpu.h"
#include "int.h"


void IntReq(FemtoEmu_t *emu)
{
    if (CHK_IRQ_ENABLE(emu))
    {
        /* ACKNOWLEDGE OF THE IRQ */
        RES_IREQ(emu)

        /* PUSH PC ON THE STACK, LOW THEN HIGH PART */
        uint8_t pc_low  = (uint8_t)(PC & 0x00FF);
        uint8_t pc_high = (uint8_t)((PC & 0x0F00) >> 8);

        StackPushByte(emu, pc_low);   /* PUSH LOW PART OF PC */
        StackPushByte(emu, pc_high);  /* PUSH HIGH PART OF PC */

        /* GOTO TO THE ADDRESS STORE IN THE IRQ VECTOR */
        PC = GET_ADDR_VEC(IREQ_VEC);
    }
}


void SysReq(FemtoEmu_t *emu)
{
    /* PUSH PC ON THE STACK, LOW THEN HIGH PART */
    uint8_t pc_low  = (uint8_t)(PC & 0x00FF);
    uint8_t pc_high = (uint8_t)((PC & 0x0F00) >> 8);

    StackPushByte(emu, pc_low);   /* PUSH LOW PART OF PC */
    StackPushByte(emu, pc_high);  /* PUSH HIGH PART OF PC */

    /* GOTO TO THE ADDRESS STORE IN THE IRQ VECTOR */
    PC = GET_ADDR_VEC(SYS_VEC);
}