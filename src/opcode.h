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

#ifndef OPCODE_H_
#define OPCODE_H_

#ifdef _ASM_
    #undef _ASM_
#endif

#ifndef _EMU_
#define _EMU_
    #include "common.h"
#endif

typedef void (*FemtoOpcode)(void);


/*** OPCODE FUNCTIONS ***/
void OpcodeError(void)
{
    printf("Invalid Opcode 0x%02X at 0x%03X!\n", inst, (pc - 3) % 0xFFF);
    halt = true;
    return;
}

void OpcodeHlt(void)
{
    printf("HLT INSTRUCTION AT 0x%03X\n", (pc - 3) % 0xFFF);
    halt = true;
    return;
}
                
void OpcodeLdr(void)
{
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
}
                
void OpcodeLdm(void)
{
    /* LDM DREG, SREG | IMM */
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
}
                
void OpcodeSti(void)
{
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
}
                
void OpcodeStr(void)
{
    /* STR IMM | REG, REG */
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
}
                
void OpcodeAdd(void)
{
    /* ADD REG, REG */
    temp = (int)r[dreg] + (int)r[sreg];
    flags = test_update_flags(temp);
    temp = r[dreg];
    r[dreg] += r[sreg];
    printf("ADD: R%d (0x%02X) = R%d (0x%02X) + R%d (0x%02X)\n", dreg, r[dreg], dreg, r[dreg] - r[sreg], sreg, r[sreg]);
    print_flags(flags);
}
                
void OpcodeSub(void)
{
    /* SUB REG, REG */
    temp = (int)r[dreg] - (int)r[sreg];
    flags = test_update_flags(temp);
    temp = r[dreg];
    r[dreg] -= r[sreg];
    printf("SUB: R%d (0x%02X) = R%d (0x%02X) - R%d (0x%02X)\n", dreg, r[dreg], dreg, temp, sreg, r[sreg]);
    print_flags(flags);
}
                
void OpcodeCmp(void)
{
    /* CMP REG, REG */
    temp = r[dreg] - r[sreg];
    flags = test_update_flags(temp);
    printf("CMP: R%d (0x%02X), R%d (0x%02X)\n", dreg, r[dreg], sreg, r[sreg]);
    print_flags(flags);
}
                
void OpcodeJmp(void)
{
    /* JMP IMM */
    pc = addr;
    printf("JMP: 0x%03X (PC = 0x%03X)\n", addr, pc);
}
                
void OpcodeJz(void)
{
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
}

void OpcodeJnz(void)
{
    /* JNZ/JNE IMM */
    if (ZFLAG == 0)
    {
        pc = addr;
        printf("JNZ/JNE: 0x%03X (PC = 0x%03X)\n", addr, pc);
    }
    else
    {
        printf("NOT TAKEN JNZ/JNE: 0x%03X (PC = 0x%03X)\n", addr, pc);
    }
}
                
void OpcodeJn(void)
{
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
}

void OpcodeJnn(void)
{
    /* JN IMM */
    if (NFLAG == 0)
    {
        pc = addr;
        printf("JNN: 0x%03X (PC = 0x%03X)\n", addr, pc);
    }
    else
    {
        printf("NOT TAKEN JNN: 0x%03X (PC = 0x%03X)\n", addr, pc);
    }
}
                
void OpcodeJc(void)
{
    /* JC IMM */
    if (CFLAG == 1)
    {
        pc = addr;
        printf("JC: 0x%03X (PC = 0x%03X)\n", addr, pc);
    }
    else
    {
        printf("NOT TAKEN JC: 0x%03X (PC = 0x%03X)\n", addr, pc);
    }
}
                
void OpcodeJnc(void)
{
    /* JNC IMM */
    if (CFLAG == 0)
    {
        pc = addr;
        printf("JNC: 0x%03X (PC = 0x%03X)\n", addr, pc);
    }
    else
    {
        printf("NOT TAKEN JNC: 0x%03X (PC = 0x%03X)\n", addr, pc);
    }
}

FemtoOpcode OpcodeFunc[0x20] =
{
    OpcodeHlt, OpcodeLdr, OpcodeLdm, OpcodeSti, OpcodeStr, OpcodeAdd, OpcodeSub, OpcodeCmp,
    OpcodeJz,  OpcodeJnz, OpcodeJn,  OpcodeJc,  OpcodeJnc, OpcodeError, OpcodeJmp, OpcodeJnn,
    OpcodeError, OpcodeError, OpcodeError, OpcodeError, OpcodeError, OpcodeError, OpcodeError, OpcodeError,
    OpcodeError, OpcodeError, OpcodeError, OpcodeError, OpcodeError, OpcodeError, OpcodeError, OpcodeError
};
#endif