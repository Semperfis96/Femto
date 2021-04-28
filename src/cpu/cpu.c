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
#include "cpu.h"
#include "../common.h"
#include "../io/io.h"


typedef void (*FemtoOpcode)(FemtoEmu_t *emu, bool verbose);


/*** HELPING FUNCTIONS ***/
uint8_t UpdateFlags(int testing)
{
    /* reset the flags */
    uint8_t temp_flags = 0;

    /* test & set in consequence */
    if (testing == 0)
    {
        /* set the zero flag */
        temp_flags = 0x1;
    }
    else if (testing < 0)
    {
        /* set the negative flag */
        temp_flags = 0x4;
    }
    else if (testing > 0xFF)
    {
        /* set the carry flags */
        temp_flags = 0x2;
    }

    return temp_flags;
}

void PrintFlags(FemtoEmu_t *emu, bool verbose)
{
    if (verbose == true) printf("FLAGS: N : %01X; C : %01X; Z : %01X\n", NFLAG, CFLAG, ZFLAG);
}

/* STACK HELPING FUNCTIONS */
void StackPushByte(FemtoEmu_t *emu, uint8_t byte)
{
    RAM[STACK_BASE + (SP++)] = byte;
}

uint8_t StackPopByte(FemtoEmu_t *emu)
{
    return RAM[STACK_BASE + (--SP)];
}
/*** END OF HELPING FUNCTIONS ***/



/*** OPCODE FUNCTIONS ***/
void OpcodeError(FemtoEmu_t *emu, bool verbose)
{
    if (verbose == true) printf("FATAL ERROR !!! ==> Invalid Opcode 0x%02X at 0x%03X!\n", INST, (PC - 3) % 0xFFF);
    printf("FATAL ERROR !!! ==> Invalid Opcode 0x%02X at 0x%03X!\n", INST, (PC - 3) % 0xFFF);
    HALT = true;
    return;
}

void OpcodeHlt(FemtoEmu_t *emu, bool verbose)
{
    if (verbose == true) printf("HLT INSTRUCTION AT 0x%03X\n", (PC - 3) % 0xFFF);
    HALT = true;
    return;
}
                
void OpcodeLdr(FemtoEmu_t *emu, bool verbose)
{
    /* LDR DREG, SREG | IMM */
    if (ADRM == ADRM_IMM)
    {
        R[DREG] = DATA;
        if (verbose == true) printf("LDR: R%d = 0x%02X\n", DREG, R[DREG]);
    }
    else if (ADRM == ADRM_REG)
    {
        R[DREG] = R[SREG];
        if (verbose == true) printf("LDR: R%d = R%d (0x%02X)\n", DREG, SREG, R[SREG]);
    }
}
                
void OpcodeLdm(FemtoEmu_t *emu, bool verbose)
{
    /* LDM DREG, SREG | IMM */
    if (ADRM == ADRM_IMM)
    {
        R[DREG] = RAM[ADDR];
        if (verbose == true) printf("LDM: R%d = 0x%02X (RAM[0x%03X])\n", DREG, R[DREG], ADDR);
    }
    else if (ADRM == ADRM_REG)
    {
        R[DREG] = RAM[R[SREG]];
        if (verbose == true) printf("LDM: R%d = 0x%02X (RAM[R%d] (0x%02X))\n", DREG, R[DREG], SREG, R[SREG]);
    }
}
                
void OpcodeSti(FemtoEmu_t *emu, bool verbose)
{
    /* STI REG, IMM */
    if (ADRM == ADRM_IMM)
    {
        RAM[R[DREG]] = DATA;
        if (verbose == true) printf("STI: RAM[R%d (0x%02X)] = 0x%02X\n", DREG, R[DREG], RAM[R[DREG]]);
    }
    else
    {
        printf("ILLEGAL ADDRESSING MODES (REGISTER) FOR STI AT 0x%03X\n", (PC - 3) % 0xFFF);
        HALT = true;
    }
}
                
void OpcodeStr(FemtoEmu_t *emu, bool verbose)
{
    /* STR IMM | REG, REG */
    if (ADRM == ADRM_IMM)
    {
        RAM[ADDR] = R[SREG];
        if (verbose == true) printf("STR: RAM[0x%03X] = 0x%02X (R%d (0x%02X))\n", ADDR, RAM[ADDR], SREG, R[SREG]);
    }
    else if (ADRM == ADRM_REG)
    {
        RAM[R[DREG]] = R[SREG];
        if (verbose == true) printf("STR: RAM[R%d (0x%02X)] = 0x%02X (R%d (0x%02X))\n", DREG, R[DREG], RAM[R[DREG]], SREG, R[SREG]);
    }
}
                
void OpcodeAdd(FemtoEmu_t *emu, bool verbose)
{
    /* ADD REG, REG */
    TEMP = (int)R[DREG] + (int)R[SREG];
    FLAGS = UpdateFlags(TEMP);
    TEMP = R[DREG];
    R[DREG] += R[SREG];
    if (verbose == true) printf("ADD: R%d (0x%02X) = R%d (0x%02X) + R%d (0x%02X)\n", DREG, R[DREG], DREG, R[DREG] - R[SREG], SREG, R[SREG]);
    PrintFlags(emu, verbose);
}
                
void OpcodeSub(FemtoEmu_t *emu, bool verbose)
{
    /* SUB REG, REG */
    TEMP = (int)R[DREG] - (int)R[SREG];
    FLAGS = UpdateFlags(TEMP);
    TEMP = R[DREG];
    R[DREG] -= R[SREG];
    if (verbose == true) printf("SUB: R%d (0x%02X) = R%d (0x%02X) - R%d (0x%02X)\n", DREG, R[DREG], DREG, TEMP, SREG, R[SREG]);
    PrintFlags(emu, verbose);
}
                
void OpcodeCmp(FemtoEmu_t *emu, bool verbose)
{
    /* CMP REG, REG */
    TEMP = R[DREG] - R[SREG];
    FLAGS = UpdateFlags(TEMP);
    if (verbose == true) printf("CMP: R%d (0x%02X), R%d (0x%02X)\n", DREG, R[DREG], SREG, R[SREG]);
    PrintFlags(emu, verbose);
}
                
void OpcodeJmp(FemtoEmu_t *emu, bool verbose)
{
    /* JMP IMM */
    PC = ADDR;
    if (verbose == true) printf("JMP TO 0x%03X (PC = 0x%03X)\n", ADDR, PC);
}
                
void OpcodeJz(FemtoEmu_t *emu, bool verbose)
{
    /* JZ/JE IMM */
    if (ZFLAG == 1)
    {
        PC = ADDR;
        if (verbose == true) printf("JZ/JE TAKEN TO 0x%03X (PC = 0x%03X)\n", ADDR, PC);
    }
    else
    {
        if (verbose == true) printf("JZ/JE NOT TAKEN TO 0x%03X (PC = 0x%03X)\n", ADDR, PC);
    }
}

void OpcodeJnz(FemtoEmu_t *emu, bool verbose)
{
    /* JNZ/JNE IMM */
    if (ZFLAG == 0)
    {
        PC = ADDR;
        if (verbose == true) printf("JNZ/JNE TAKEN TO 0x%03X (PC = 0x%03X)\n", ADDR, PC);
    }
    else
    {
        if (verbose == true) printf("JNZ/JNE NOT TAKEN TO 0x%03X (PC = 0x%03X)\n", ADDR, PC);
    }
}
                
void OpcodeJn(FemtoEmu_t *emu, bool verbose)
{
    /* JN IMM */
    if (NFLAG == 1)
    {
        PC = ADDR;
        if (verbose == true) printf("JN TAKEN TO 0x%03X (PC = 0x%03X)\n", ADDR, PC);
    }
    else
    {
        if (verbose == true) printf("JN NOT TAKEN TO 0x%03X (PC = 0x%03X)\n", ADDR, PC);
    }
}

void OpcodeJnn(FemtoEmu_t *emu, bool verbose)
{
    /* JN IMM */
    if (NFLAG == 0)
    {
        PC = ADDR;
        if (verbose == true) printf("JNN TAKEN TO 0x%03X (PC = 0x%03X)\n", ADDR, PC);
    }
    else
    {
        if (verbose == true) printf("JNN NOT TAKEN TO 0x%03X (PC = 0x%03X)\n", ADDR, PC);
    }
}
                
void OpcodeJc(FemtoEmu_t *emu, bool verbose)
{
    /* JC IMM */
    if (CFLAG == 1)
    {
        PC = ADDR;
        if (verbose == true) printf("JC TAKEN TO 0x%03X (PC = 0x%03X)\n", ADDR, PC);
    }
    else
    {
        if (verbose == true) printf("JC NOT TAKEN TO 0x%03X (PC = 0x%03X)\n", ADDR, PC);
    }
}
                
void OpcodeJnc(FemtoEmu_t *emu, bool verbose)
{
    /* JNC IMM */
    if (CFLAG == 0)
    {
        PC = ADDR;
        if (verbose == true) printf("JNC TAKEN TO 0x%03X (PC = 0x%03X)\n", ADDR, PC);
    }
    else
    {
        if (verbose == true) printf("JNCNOT TAKEN TO 0x%03X (PC = 0x%03X)\n", ADDR, PC);
    }
}

void OpcodeJbe(FemtoEmu_t *emu, bool verbose)
{
    /* JBE IMM */
    if ((CFLAG == 1) || (ZFLAG == 1))
    {
        PC = ADDR;
        if (verbose == true) printf("JBE TAKEN TO 0x%03X (PC = 0x%03X)\n", ADDR, PC);
    }
    else
    {
        if (verbose == true) printf("JBE NOT TAKEN TO 0x%03X (PC = 0x%03X)\n", ADDR, PC);
    }
}

void OpcodeJa(FemtoEmu_t *emu, bool verbose)
{
    /* JA IMM */
    if (((!CFLAG) == 1) && ((!ZFLAG) == 1))
    {
        PC = ADDR;
        if (verbose == true) printf("JA TAKEN TO 0x%03X (PC = 0x%03X)\n", ADDR, PC);
    }
    else
    {
        if (verbose == true) printf("JA NOT TAKEN TO 0x%03X (PC = 0x%03X)\n", ADDR, PC);
    }
}

void OpcodePush(FemtoEmu_t *emu, bool verbose)
{
    /* PUSH REG | IMM */
    if (ADRM == ADRM_REG)
    {
        StackPushByte(emu, R[DREG]);
        if (verbose == true) printf("PUSH R%d (0x%02X); SP = 0x%02X\n", DREG, R[DREG], SP);
    }
    else
    {
        StackPushByte(emu, DATA);
        if (verbose == true) printf("PUSH 0x%02X; SP = 0x%02X\n", DATA, SP);
    }
}

void OpcodePop(FemtoEmu_t *emu, bool verbose)
{
    /* POP REG */
    R[DREG] = StackPopByte(emu);
    if (verbose == true) printf("POP IN R%d (0x%02X); SP = 0x%02X\n", DREG, R[DREG], SP);
}

void OpcodeCall(FemtoEmu_t *emu, bool verbose)
{
    /* CALL IMM */
    uint8_t pc_low  = (uint8_t)(PC & 0x00FF);
    uint8_t pc_high = (uint8_t)((PC & 0x0F00) >> 8);

    StackPushByte(emu, pc_low);   /* PUSH LOW PART OF PC */
    StackPushByte(emu, pc_high);  /* PUSH HIGH PART OF PC */
    PC = ADDR;
    if (verbose == true) printf("CALL TO 0x%03X; LOW PC = 0x%02X & HIGH PC = 0x%01X\n", ADDR, pc_low, pc_high);
}

void OpcodeRet(FemtoEmu_t *emu, bool verbose)
{
    /* RET */
    uint8_t pc_high = StackPopByte(emu);
    uint8_t pc_low  = StackPopByte(emu);

    PC = (pc_high << 8) | pc_low;
    if (verbose == true) printf("RET TO 0x%03X; LOW PC = 0x%02X & HIGH PC = 0x%01X\n", PC, pc_low, pc_high); 

}

void OpcodeIn(FemtoEmu_t *emu, bool verbose)
{
    /* IN REG, REG | IMM */
    if (ADRM == ADRM_IMM)
    {
        R[DREG] = In(DATA);
        if (verbose == true) printf("IN FROM PORT 0x%02X TO R%d (=0x%02X)\n", DATA, DREG, R[DREG]);
    }
    else
    {
        R[DREG] = In(R[SREG]);
        if (verbose == true) printf("IN FROM PORT R%d (=0x%02X) TO R%d (=0x%02X)\n", SREG, R[SREG], DREG, R[DREG]);
    }
}

void OpcodeOut(FemtoEmu_t *emu, bool verbose)
{
    /* OUT REG | IMM, REG */
    if (ADRM == ADRM_IMM)
    {
        Out(DATA, R[SREG]);
        if (verbose == true) printf("OUT TO PORT 0x%02X FROM R%d (=0x%02X)\n", DATA, SREG, R[SREG]);
    }
    else
    {
        Out(R[DREG], R[SREG]);
        if (verbose == true) printf("OUT TO PORT R%d (=0x%02X) FROM R%d (=0x%02X)\n", DREG, R[DREG], SREG, R[SREG]);
    }
}
/*** END OF OPCODE FUNCTIONS ***/


/*** OPCODE FUNCTION POINTER ARRAY, BETTER THAN INTERPRETED OR SWITCH STATEMENT EMULATION ***/
FemtoOpcode OpcodeFunc[0x20] =
{
    OpcodeHlt,   OpcodeLdr,   OpcodeLdm,   OpcodeSti,   OpcodeStr,   OpcodeAdd,   OpcodeSub,   OpcodeCmp,
    OpcodeJz,    OpcodeJn,    OpcodeJc,    OpcodeJnc,   OpcodeJbe,   OpcodeJa,    OpcodeJmp,   OpcodeJnz,
    OpcodeJnn,   OpcodePush,  OpcodePop,   OpcodeCall,  OpcodeRet,   OpcodeIn,    OpcodeOut,   OpcodeError,
    OpcodeError, OpcodeError, OpcodeError, OpcodeError, OpcodeError, OpcodeError, OpcodeError, OpcodeError
};


void CpuExecInst(FemtoEmu_t *emu, bool verbose)
{
     /* FETCH INSTRUCTION FROM RAM */
    if (verbose == true) printf("[0x%03X] ",  PC);
    F[0] = RAM[(PC++ % 0xFFF)];
    F[1] = RAM[(PC++ % 0xFFF)];
    F[2] = RAM[(PC++ % 0xFFF)];

    /* DECODE INSTRUCTION */
    INST =   F[0] & 0x7F;
    ADRM =  (F[0] & 0x80) >> 7;
    DREG =  (F[1] >> 6) & 0x03;
    SREG =  (F[1] >> 4) & 0x03;
    DATA =   F[2];
    ADDR = ((F[1] & 0x0F) << 8) | F[2];

    /* EXECUTE INSTRUCTION, CALL THE APPROPRIATE FUNCTION THAT EMULATE THE OPCODE */
    (*OpcodeFunc[INST])(emu, verbose);
    TEMP = 0;
}