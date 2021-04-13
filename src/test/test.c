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
#include "../opcode.h"
#include "test.h"

#ifdef _ASM_
    #undef _ASM_
#endif

#ifndef _EMU_
#define _EMU_
    #include "../common.h"
#endif



/*** CMD FUNCTIONS ***/
void cmd_version(void)
{
    printf("test %s | Copyright (C) 2021 Semperfis\n", (char *)TEST_VERSION);
    printf("This program comes with ABSOLUTELY NO WARRANTY;\n");
    printf("License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>;\n");
    printf("This is free software, and you are welcome to redistribute it under certain conditions;\n\n");
}
/*** CMD FUNCTIONS END ***/


void ResetVar(void)
{
    pc    = 0x0;       /* PROGRAM COUNTER (12BITS) */
    r[0]  = 0x0;       /* GP REGISTERS (R0, R1, R2 AND R3) */
    r[1]  = 0x0;       /* GP REGISTERS (R0, R1, R2 AND R3) */
    r[2]  = 0x0;       /* GP REGISTERS (R0, R1, R2 AND R3) */
    r[3]  = 0x0;       /* GP REGISTERS (R0, R1, R2 AND R3) */
    flags = 0x0;       /* FLAGS REGISTER */
    adrm  = ADRM_IMM;  /* ADDRESSING MODE */
    halt  = false;     /* CPU IS HALT OR NOT */
    data  = 0;         /* 8BITS DATA */
    addr  = 0;         /* 12BITS ADDRESS */
    dreg  = 0;         /* DESTINATION REGISTER */
    sreg  = 0;         /* SOURCE REGISTER */
}


/*** UNIT TESTING FUNCTIONS ***/
void TestOpcodeHlt(void)
{
    OpcodeHlt();
    ASSERT_EQ(halt, true, "HLT")
    ResetVar();
}

void TestOpcodeLdr(void)
{
    /* Test for immediate & for register addressing */
    dreg = 0; /* R0 */
    sreg = 1; /* R1 */
    data = 0xFF;
    r[sreg] = 0xAA;
    adrm = ADRM_IMM;

    OpcodeLdr();
    ASSERT_EQ(r[dreg], 0xFF, "LDR (ADRM_IMM)")

    adrm = ADRM_REG;
    OpcodeLdr();
    ASSERT_EQ(r[dreg], 0xAA, "LDR (ADRM_REG)")
    ResetVar();
}
/*** END OF UNIT TESTING FUNCTIONS ***/


/*** PROGRAM ENTRY POINT ***/
int main(void)
{
    cmd_version();

    /* EXECUTE TEST */
    TestOpcodeHlt();
    TestOpcodeLdr();

    return 0;
}