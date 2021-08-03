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
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "../femto.h"
#include "../common.h"
#include "../cpu/cpu.h"
#include "../io/io.h"
#include "../cpu/int.h"
#include "test.h"


/*** CMD FUNCTIONS ***/
void cmd_version(void)
{
    printf("test %s | Copyright (C) 2021 Semperfis\n", (char *)TEST_VERSION);
    printf("This program comes with ABSOLUTELY NO WARRANTY;\n");
    printf("License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>;\n");
    printf("This is free software, and you are welcome to redistribute it under certain conditions;\n\n");
}
/*** CMD FUNCTIONS END ***/


void ResetVar(FemtoEmu_t *emu)
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


/*** UNIT TESTING FUNCTIONS ***/
void TestOpcodeHlt(FemtoEmu_t *emu)
{
    OpcodeHlt(emu, false);
    ASSERT_EQ(HALT, true, "HLT")
    ResetVar(emu);
}

void TestOpcodeLdr(FemtoEmu_t *emu)
{
    /* Test for immediate & for register ADDRessing */
    DREG = 0; /* R0 */
    SREG = 1; /* R1 */
    DATA = 0xFF;
    R[SREG] = 0xAA;
    ADRM = ADRM_IMM;

    OpcodeLdr(emu, false);
    ASSERT_EQ(R[DREG], 0xFF, "LDR (ADRM_IMM)")

    ADRM = ADRM_REG;
    OpcodeLdr(emu, false);
    ASSERT_EQ(R[DREG], 0xAA, "LDR (ADRM_REG)")
    ResetVar(emu);
}

void TestOpcodeLdm(FemtoEmu_t *emu)
{
    /* Test for immediate & for register ADDRessing */
    DREG = 0; /* R0 */
    SREG = 1; /* R1 */
    ADDR = 0xEC;
    R[SREG] = ADDR;
    RAM[ADDR] = 0xBA;
    ADRM = ADRM_IMM;

    OpcodeLdm(emu, false);
    ASSERT_EQ(R[DREG], 0xBA, "LDM (ADRM_IMM)")
    R[DREG] = 0;

    ADRM = ADRM_REG;
    OpcodeLdm(emu, false);
    ASSERT_EQ(R[DREG], 0xBA, "LDM (ADRM_REG)")
    ResetVar(emu);
}

void TestOpcodeSti(FemtoEmu_t *emu)
{
    DREG = 0; /* R0 */
    ADDR = 0xEC;
    R[DREG] = ADDR;
    DATA = 0xFB;
    ADRM = ADRM_IMM;

    OpcodeSti(emu, false);
    ASSERT_EQ(RAM[ADDR], 0xFB, "STI")
}

void TestOpcodeStr(FemtoEmu_t *emu)
{
    /* Test for immediate & for register ADDRessing */
    DREG = 0; /* R0 */
    SREG = 1; /* R1 */
    ADDR = 0xEC;
    R[DREG] = ADDR;
    R[SREG] = 0xAD;
    ADRM = ADRM_IMM;

    OpcodeStr(emu, false);
    ASSERT_EQ(RAM[ADDR], 0xAD, "STR (ADRM_IMM)")
    RAM[ADDR] = 0;

    ADRM = ADRM_REG;
    OpcodeStr(emu, false);
    ASSERT_EQ(RAM[ADDR], 0xAD, "STR (ADRM_REG)")
    ResetVar(emu);
}

void TestOpcodeAdd(FemtoEmu_t *emu)
{
    DREG = 0;
    SREG = 1;
    R[DREG] = 20;
    R[SREG] = 71;
    ADRM = ADRM_REG;

    OpcodeAdd(emu, false);
    ASSERT_EQ(R[DREG], 91, "ADD")

    R[DREG] = 255;
    R[SREG] = 2;
    OpcodeAdd(emu, false);
    ASSERT_EQ(CFLAG, 1, "ADD (CFLAG)")
    ResetVar(emu);
}

void TestOpcodeSub(FemtoEmu_t *emu)
{
    DREG = 0;
    SREG = 1;
    R[DREG] = 89;
    R[SREG] = 17;
    ADRM = ADRM_REG;

    OpcodeSub(emu, false);
    ASSERT_EQ(R[DREG], (89-17), "SUB")

    R[DREG] = 58;
    R[SREG] = 192;
    OpcodeSub(emu, false);
    ASSERT_EQ(NFLAG, 1, "SUB (NFLAG)")
    ResetVar(emu);
}

void TestOpcodeCmp(FemtoEmu_t *emu)
{
    DREG = 0;
    SREG = 1;
    R[DREG] = 34;
    R[SREG] = 99;
    ADRM = ADRM_REG;

    OpcodeCmp(emu, false);
    ASSERT_EQ(NFLAG, 1, "CMP (NFLAG)")

    R[DREG] = 77;
    R[SREG] = 77;
    OpcodeCmp(emu, false);
    ASSERT_EQ(ZFLAG, 1, "CMP (ZFLAG)")
    ResetVar(emu);
}

void TestOpcodeJmp(FemtoEmu_t *emu)
{
    ADDR = 0xCAD;
    ADRM = ADRM_IMM;

    OpcodeJmp(emu, false);
    ASSERT_EQ(PC, 0xCAD, "JMP")
    ResetVar(emu);
}

void TestOpcodeJz(FemtoEmu_t *emu)
{
    ADDR = 0xF4A;
    ADRM = ADRM_IMM;

    OpcodeJz(emu, false);
    ASSERT_EQ(PC, 0, "JZ NOT TAKEN (ZFLAG = 0)")

    FLAGS = 0x1;
    OpcodeJz(emu, false);
    ASSERT_EQ(PC, 0xF4A, "JZ TAKEN (ZFLAG = 1)")
    ResetVar(emu);
}

void TestOpcodeJnz(FemtoEmu_t *emu)
{
    ADDR = 0xF4A;
    ADRM = ADRM_IMM;

    OpcodeJnz(emu, false);
    ASSERT_EQ(PC, 0xF4A, "JNZ TAKEN (ZFLAG = 0)")

    FLAGS = 0x1;
    PC = 0;
    OpcodeJnz(emu, false);
    ASSERT_EQ(PC, 0, "JNZ NOT TAKEN (ZFLAG = 1)")
    ResetVar(emu);
}

void TestOpcodeJc(FemtoEmu_t *emu)
{
    ADDR = 0xF4A;
    ADRM = ADRM_IMM;

    OpcodeJc(emu, false);
    ASSERT_EQ(PC, 0, "JC NOT TAKEN (CFLAG = 0)")

    FLAGS = 0x2;
    OpcodeJc(emu, false);
    ASSERT_EQ(PC, 0xF4A, "JC TAKEN (CFLAG = 1)")
    ResetVar(emu);
}

void TestOpcodeJnc(FemtoEmu_t *emu)
{
    ADDR = 0xF4A;
    ADRM = ADRM_IMM;

    OpcodeJnc(emu, false);
    ASSERT_EQ(PC, 0xF4A, "JNC TAKEN (CFLAG = 0)")

    FLAGS = 0x2;
    PC = 0;
    OpcodeJnc(emu, false);
    ASSERT_EQ(PC, 0, "JNC NOT TAKEN (CFLAG = 1)")
    ResetVar(emu);
}

void TestOpcodeJn(FemtoEmu_t *emu)
{
    ADDR = 0xF4A;
    ADRM = ADRM_IMM;

    OpcodeJn(emu, false);
    ASSERT_EQ(PC, 0, "JN NOT TAKEN (NFLAG = 0)")

    FLAGS = 0x4;
    OpcodeJn(emu, false);
    ASSERT_EQ(PC, 0xF4A, "JN TAKEN (NFLAG = 1)")
    ResetVar(emu);
}

void TestOpcodeJnn(FemtoEmu_t *emu)
{
    ADDR = 0xF4A;
    ADRM = ADRM_IMM;

    OpcodeJnn(emu, false);
    ASSERT_EQ(PC, 0xF4A, "JNN TAKEN (NFLAG = 0)")

    FLAGS = 0x4;
    PC = 0;
    OpcodeJnn(emu, false);
    ASSERT_EQ(PC, 0, "JNN NOT TAKEN (NFLAG = 1)")
    ResetVar(emu);
}

void TestOpcodeJbe(FemtoEmu_t *emu)
{
    ADDR = 0xF4A;
    ADRM = ADRM_IMM;

    OpcodeJbe(emu, false);
    ASSERT_EQ(PC, 0, "JBE NOT TAKEN (CFLAG = 0 AND ZFLAG = 0)")

    FLAGS = 0x2;
    PC = 0;
    OpcodeJbe(emu, false);
    ASSERT_EQ(PC, 0xF4A, "JBE TAKEN (CFLAG = 1 AND ZFLAG = 0)")

    FLAGS = 0x1;
    PC = 0;
    OpcodeJbe(emu, false);
    ASSERT_EQ(PC, 0xF4A, "JBE TAKEN (CFLAG = 0 AND ZFLAG = 1)")


    FLAGS = 0x3;
    PC = 0;
    OpcodeJbe(emu, false);
    ASSERT_EQ(PC, 0xF4A, "JBE TAKEN (CFLAG = 1 AND ZFLAG = 1)")
    ResetVar(emu);
}

void TestOpcodeJa(FemtoEmu_t *emu)
{
    ADDR = 0xF4A;
    ADRM = ADRM_IMM;

    OpcodeJa(emu, false);
    ASSERT_EQ(PC, 0xF4A, "JA TAKEN (CFLAG = 0 (1) AND ZFLAG = 0 (1))")

    FLAGS = 0x2;
    PC = 0;
    OpcodeJa(emu, false);
    ASSERT_EQ(PC, 0, "JA NOT TAKEN (CFLAG = 1 (0) AND ZFLAG = 0 (1))")

    FLAGS = 0x1;
    PC = 0;
    OpcodeJa(emu, false);
    ASSERT_EQ(PC, 0, "JA NOT TAKEN (CFLAG = 0 (1) AND ZFLAG = 1 (0))")


    FLAGS = 0x3;
    PC = 0;
    OpcodeJa(emu, false);
    ASSERT_EQ(PC, 0, "JA NOT TAKEN (CFLAG = 1 (0) AND ZFLAG = 1 (0))")
    ResetVar(emu);
}

void TestOpcodePush(FemtoEmu_t *emu)
{
    DATA    = 0xEA;
    DREG    = 0;
    R[DREG] = 0x74;
    ADRM    = ADRM_IMM;

    OpcodePush(emu, false);
    ASSERT_EQ(RAM[STACK_BASE + (--SP)], 0xEA, "PUSH (IMM)")

    ADRM = ADRM_REG;
    SP = 0;
    OpcodePush(emu, false);
    ASSERT_EQ(RAM[STACK_BASE + (--SP)], 0x74, "PUSH (REG)")
    ResetVar(emu);
}

void TestOpcodePop(FemtoEmu_t *emu)
{
    DATA    = 0x9E;
    DREG    = 0;
    ADRM    = ADRM_IMM;

    OpcodePush(emu, false);
    
    ADRM = ADRM_REG;
    OpcodePop(emu, false);
    ASSERT_EQ(R[DREG], 0x9E, "POP")
    ResetVar(emu);
}

void TestOpcodeCall(FemtoEmu_t *emu)
{
    PC   = 0x379;
    ADDR = 0x666;
    ADRM = ADRM_IMM;

    OpcodeCall(emu, false);
    ASSERT_EQ(PC, 0x666, "CALL (NEW PC CHECK)")
    ASSERT_EQ(StackPopByte(emu), 0x3, "CALL (PC HIGH STACK CHECK)")
    ASSERT_EQ(StackPopByte(emu), 0x79, "CALL (PC LOW STACK CHECK)")
    ResetVar(emu);
}

void TestOpcodeRet(FemtoEmu_t *emu)
{
    PC = 0xA0E;

    StackPushByte(emu, 0x79);
    StackPushByte(emu, 0x3);
    OpcodeRet(emu, false);
    ASSERT_EQ(PC, 0x379, "RET")

    ResetVar(emu);
}


/*** IO OPCODE TESTING ***/
uint8_t TestInFunc(void)
{
    return 0x91;
}

void TestOutFunc(uint8_t data)
{
    ASSERT_EQ(data, 0xAF, "OUT");
}


void TestOpcodeIn(FemtoEmu_t *emu)
{
    ADRM = ADRM_IMM;
    DREG = 0;
    DATA = 0x10;
    SREG = 1;
    R[SREG] = 0x10;

    RegisterInputFunc(TestInFunc, 0x10);
    ASSERT_EQ(In(0x10), 0x91, "IN (IMM)")

    ADRM = ADRM_REG;
    R[DREG] = 0;
    ASSERT_EQ(In(0x10), 0x91, "IN (REG)")

    ResetVar(emu);
}

void TestOpcodeOut(FemtoEmu_t *emu)
{
    ADRM = ADRM_IMM;
    DREG = 1;
    SREG = 0;
    R[SREG] = 0xAC;
    R[DREG] = 0x78;
    DATA    = 0x78;

    RegisterOutputFunc(TestOutFunc, 0x78);
    Out(0xAF, 0x78);

    ADRM = ADRM_REG;
    Out(0xAF, 0x78);

    ResetVar(emu);
}
/*** END OF IO OPCODE TESTING ***/

void TestOpcodeSys(FemtoEmu_t *emu)
{
    RAM[0x02] = 0xFF;   /* LOW SYS VECTOR PART */
    RAM[0x03] = 0x01;   /* HIGH SYS VECTOR PART */

    OpcodeSys(emu, false);
    ASSERT_EQ(PC, 0x01FF, "SYS")
    ResetVar(emu);
}

void TestOpcodeSei(FemtoEmu_t *emu)
{
    OpcodeSei(emu, false);
    ASSERT_EQ(IFLAG, 1, "SEI")
    ASSERT_EQ(CHK_IRQ_ENABLE(emu), 1, "IRQ ENABLE")
    ResetVar(emu);
}

void TestOpcodeSdi(FemtoEmu_t *emu)
{
    OpcodeSdi(emu, false);
    ASSERT_EQ(IFLAG, 0, "SDI")
    ASSERT_EQ(CHK_IRQ_ENABLE(emu), 0, "IRQ DISABLE")
    ResetVar(emu);
}

/*** END OF UNIT TESTING FUNCTIONS ***/


/*** PROGRAM ENTRY POINT ***/
int main(void)
{
    FemtoEmu_t *test_emu = NULL;


    cmd_version();

    /* EMULATION STATE ALLOCATION */
    test_emu = malloc(sizeof(FemtoEmu_t));
    if (test_emu == NULL)
    {
        printf("ERROR (main): CAN'T ALLOCATE EMULATION STATE!!!\n");
        exit(-1);
    }
    ResetVar(test_emu);


    /* RAM ALLOCATION */
    test_emu->ram = malloc(4 * 1024 * sizeof(uint8_t));
    if (test_emu->ram == NULL)
    {
        printf("ERROR (main): CAN'T ALLOCATE VIRTUAL RAM !!!\n");
        free(test_emu);
        exit(-1);
    }


    /* EXECUTE TEST */
    TestOpcodeHlt(test_emu);
    TestOpcodeLdr(test_emu);
    TestOpcodeLdm(test_emu);
    TestOpcodeSti(test_emu);
    TestOpcodeStr(test_emu);
    TestOpcodeAdd(test_emu);
    TestOpcodeSub(test_emu);
    TestOpcodeCmp(test_emu);
    TestOpcodeJmp(test_emu);
    TestOpcodeJz(test_emu);
    TestOpcodeJnz(test_emu);
    TestOpcodeJc(test_emu);
    TestOpcodeJnc(test_emu);
    TestOpcodeJn(test_emu);
    TestOpcodeJnn(test_emu);
    TestOpcodeJbe(test_emu);
    TestOpcodeJa(test_emu);
    TestOpcodePush(test_emu);
    TestOpcodePop(test_emu);
    TestOpcodeCall(test_emu);
    TestOpcodeRet(test_emu);
    TestOpcodeIn(test_emu);
    TestOpcodeOut(test_emu);
    TestOpcodeSys(test_emu);
    TestOpcodeSei(test_emu);
    TestOpcodeSdi(test_emu);

    return 0;
}