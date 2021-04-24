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

void TestOpcodeLdm(void)
{
    /* Test for immediate & for register addressing */
    dreg = 0; /* R0 */
    sreg = 1; /* R1 */
    addr = 0xEC;
    r[sreg] = addr;
    ram[addr] = 0xBA;
    adrm = ADRM_IMM;

    OpcodeLdm();
    ASSERT_EQ(r[dreg], 0xBA, "LDM (ADRM_IMM)")
    r[dreg] = 0;

    adrm = ADRM_REG;
    OpcodeLdm();
    ASSERT_EQ(r[dreg], 0xBA, "LDM (ADRM_REG)")
    ResetVar();
}

void TestOpcodeSti(void)
{
    dreg = 0; /* R0 */
    addr = 0xEC;
    r[dreg] = addr;
    data = 0xFB;
    adrm = ADRM_IMM;

    OpcodeSti();
    ASSERT_EQ(ram[addr], 0xFB, "STI")
}

void TestOpcodeStr(void)
{
    /* Test for immediate & for register addressing */
    dreg = 0; /* R0 */
    sreg = 1; /* R1 */
    addr = 0xEC;
    r[dreg] = addr;
    r[sreg] = 0xAD;
    adrm = ADRM_IMM;

    OpcodeStr();
    ASSERT_EQ(ram[addr], 0xAD, "STR (ADRM_IMM)")
    ram[addr] = 0;

    adrm = ADRM_REG;
    OpcodeStr();
    ASSERT_EQ(ram[addr], 0xAD, "STR (ADRM_REG)")
    ResetVar();
}

void TestOpcodeAdd(void)
{
    dreg = 0;
    sreg = 1;
    r[dreg] = 20;
    r[sreg] = 71;
    adrm = ADRM_REG;

    OpcodeAdd();
    ASSERT_EQ(r[dreg], 91, "ADD")

    r[dreg] = 255;
    r[sreg] = 2;
    OpcodeAdd();
    ASSERT_EQ(CFLAG, 1, "ADD (CFLAG)")
    ResetVar();
}

void TestOpcodeSub(void)
{
    dreg = 0;
    sreg = 1;
    r[dreg] = 89;
    r[sreg] = 17;
    adrm = ADRM_REG;

    OpcodeSub();
    ASSERT_EQ(r[dreg], (89-17), "SUB")

    r[dreg] = 58;
    r[sreg] = 192;
    OpcodeSub();
    ASSERT_EQ(NFLAG, 1, "SUB (NFLAG)")
    ResetVar();
}

void TestOpcodeCmp(void)
{
    dreg = 0;
    sreg = 1;
    r[dreg] = 34;
    r[sreg] = 99;
    adrm = ADRM_REG;

    OpcodeCmp();
    ASSERT_EQ(NFLAG, 1, "CMP (NFLAG)")

    r[dreg] = 77;
    r[sreg] = 77;
    OpcodeCmp();
    ASSERT_EQ(ZFLAG, 1, "CMP (ZFLAG)")
    ResetVar();
}

void TestOpcodeJmp(void)
{
    addr = 0xCAD;
    adrm = ADRM_IMM;

    OpcodeJmp();
    ASSERT_EQ(pc, 0xCAD, "JMP")
    ResetVar();
}

void TestOpcodeJz(void)
{
    addr = 0xF4A;
    adrm = ADRM_IMM;

    OpcodeJz();
    ASSERT_EQ(pc, 0, "JZ NOT TAKEN (ZFLAG = 0)")

    flags = 0x1;
    OpcodeJz();
    ASSERT_EQ(pc, 0xF4A, "JZ TAKEN (ZFLAG = 1)")
    ResetVar();
}

void TestOpcodeJnz(void)
{
    addr = 0xF4A;
    adrm = ADRM_IMM;

    OpcodeJnz();
    ASSERT_EQ(pc, 0xF4A, "JNZ TAKEN (ZFLAG = 0)")

    flags = 0x1;
    pc = 0;
    OpcodeJnz();
    ASSERT_EQ(pc, 0, "JNZ NOT TAKEN (ZFLAG = 1)")
    ResetVar();
}

void TestOpcodeJc(void)
{
    addr = 0xF4A;
    adrm = ADRM_IMM;

    OpcodeJc();
    ASSERT_EQ(pc, 0, "JC NOT TAKEN (CFLAG = 0)")

    flags = 0x2;
    OpcodeJc();
    ASSERT_EQ(pc, 0xF4A, "JC TAKEN (CFLAG = 1)")
    ResetVar();
}

void TestOpcodeJnc(void)
{
    addr = 0xF4A;
    adrm = ADRM_IMM;

    OpcodeJnc();
    ASSERT_EQ(pc, 0xF4A, "JNC TAKEN (CFLAG = 0)")

    flags = 0x2;
    pc = 0;
    OpcodeJnc();
    ASSERT_EQ(pc, 0, "JNC NOT TAKEN (CFLAG = 1)")
    ResetVar();
}

void TestOpcodeJn(void)
{
    addr = 0xF4A;
    adrm = ADRM_IMM;

    OpcodeJn();
    ASSERT_EQ(pc, 0, "JN NOT TAKEN (NFLAG = 0)")

    flags = 0x4;
    OpcodeJn();
    ASSERT_EQ(pc, 0xF4A, "JN TAKEN (NFLAG = 1)")
    ResetVar();
}

void TestOpcodeJnn(void)
{
    addr = 0xF4A;
    adrm = ADRM_IMM;

    OpcodeJnn();
    ASSERT_EQ(pc, 0xF4A, "JNN TAKEN (NFLAG = 0)")

    flags = 0x4;
    pc = 0;
    OpcodeJnn();
    ASSERT_EQ(pc, 0, "JNN NOT TAKEN (NFLAG = 1)")
    ResetVar();
}

void TestOpcodeJbe(void)
{
    addr = 0xF4A;
    adrm = ADRM_IMM;

    OpcodeJbe();
    ASSERT_EQ(pc, 0, "JBE NOT TAKEN (CFLAG = 0 AND ZFLAG = 0)")

    flags = 0x2;
    pc = 0;
    OpcodeJbe();
    ASSERT_EQ(pc, 0xF4A, "JBE TAKEN (CFLAG = 1 AND ZFLAG = 0)")

    flags = 0x1;
    pc = 0;
    OpcodeJbe();
    ASSERT_EQ(pc, 0xF4A, "JBE TAKEN (CFLAG = 0 AND ZFLAG = 1)")


    flags = 0x3;
    pc = 0;
    OpcodeJbe();
    ASSERT_EQ(pc, 0xF4A, "JBE TAKEN (CFLAG = 1 AND ZFLAG = 1)")
    ResetVar();
}

void TestOpcodeJa(void)
{
    addr = 0xF4A;
    adrm = ADRM_IMM;

    OpcodeJa();
    ASSERT_EQ(pc, 0xF4A, "JA TAKEN (CFLAG = 0 (1) AND ZFLAG = 0 (1))")

    flags = 0x2;
    pc = 0;
    OpcodeJa();
    ASSERT_EQ(pc, 0, "JA NOT TAKEN (CFLAG = 1 (0) AND ZFLAG = 0 (1))")

    flags = 0x1;
    pc = 0;
    OpcodeJa();
    ASSERT_EQ(pc, 0, "JA NOT TAKEN (CFLAG = 0 (1) AND ZFLAG = 1 (0))")


    flags = 0x3;
    pc = 0;
    OpcodeJa();
    ASSERT_EQ(pc, 0, "JA NOT TAKEN (CFLAG = 1 (0) AND ZFLAG = 1 (0))")
    ResetVar();
}

void TestOpcodePush(void)
{
    data    = 0xEA;
    dreg    = 0;
    r[dreg] = 0x74;
    adrm    = ADRM_IMM;

    OpcodePush();
    ASSERT_EQ(ram[STACK_BASE + (--sp)], 0xEA, "PUSH (IMM)")

    adrm = ADRM_REG;
    sp = 0;
    OpcodePush();
    ASSERT_EQ(ram[STACK_BASE + (--sp)], 0x74, "PUSH (REG)")
    ResetVar();
}

void TestOpcodePop(void)
{
    data    = 0x9E;
    dreg    = 0;
    adrm    = ADRM_IMM;

    OpcodePush();
    
    adrm = ADRM_REG;
    OpcodePop();
    ASSERT_EQ(r[dreg], 0x9E, "POP")
    ResetVar();
}

void TestOpcodeCall(void)
{
    pc   = 0x379;
    addr = 0x666;
    adrm = ADRM_IMM;

    OpcodeCall();
    ASSERT_EQ(pc, 0x666, "CALL (NEW PC CHECK)")
    ASSERT_EQ(StackPopByte(), 0x3, "CALL (PC HIGH STACK CHECK)")
    ASSERT_EQ(StackPopByte(), 0x79, "CALL (PC LOW STACK CHECK)")
    ResetVar();
}

void TestOpcodeRet(void)
{
    pc = 0xA0E;

    StackPushByte(0x79);
    StackPushByte(0x3);
    OpcodeRet();
    ASSERT_EQ(pc, 0x379, "RET")

    ResetVar();
}
/*** END OF UNIT TESTING FUNCTIONS ***/


/*** PROGRAM ENTRY POINT ***/
int main(void)
{
    cmd_version();

    ram = malloc(4096 * sizeof(uint8_t));
    if (ram == NULL)
    {
        printf("ERROR: CAN'T ALLOCATE RAM !!!\n");
        return -1;
    }

    /* EXECUTE TEST */
    TestOpcodeHlt();
    TestOpcodeLdr();
    TestOpcodeLdm();
    TestOpcodeSti();
    TestOpcodeStr();
    TestOpcodeAdd();
    TestOpcodeSub();
    TestOpcodeCmp();
    TestOpcodeJmp();
    TestOpcodeJz();
    TestOpcodeJnz();
    TestOpcodeJc();
    TestOpcodeJnc();
    TestOpcodeJn();
    TestOpcodeJnn();
    TestOpcodeJbe();
    TestOpcodeJa();
    TestOpcodePush();
    TestOpcodePop();
    TestOpcodeCall();
    TestOpcodeRet();

    return 0;
}