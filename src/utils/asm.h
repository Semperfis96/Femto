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

#ifndef ASM_H_
#define ASM_H_

typedef enum inst
{
    HLT  = 0x00,
    LDR  = 0x01,
    LDM  = 0x02,
    STI  = 0x03,
    STR  = 0x04,
    ADD  = 0x05,
    SUB  = 0x06,
    CMP  = 0x07,
    JZ   = 0x08, /* JUMP IF ZERO; SAME AS JE "x == y" */
    JN   = 0x09, /* JUMP IF NEGATIVE */
    JC   = 0x0A, /* JUMP IF CARRY; SAME AS JB (JUMP IF BELOW) "x < y" */
    JNC  = 0x0B, /* JUMP IF NOT CARRY; SAME AS JAE (JUMP IF ABOVE OR EQUAL) " x >= y" */
    JBE  = 0x0C, /* JUMP IF BELOW OR EQUAL (CARRY OR ZERO) "x <= y */
    JA   = 0x0D, /* JUMP IF ABOVE (!CARRY AND !ZERO) "x > y" */
    JMP  = 0x0E,
    JNZ  = 0x0F, /* JUMP IF NOT ZERO; SAME AS JNE "x != y" */
    JNN  = 0x10, /* JUMP IF NOT NEGATIVE */
    PUSH = 0x11,
    POP  = 0x12,
    CALL = 0x13,
    RET  = 0x14,
    IN   = 0x15,
    OUT  = 0x16
} inst_t;

typedef enum field_adrm
{
    NONE,   /* NO FIELD */
    REG,    /* FIELD SUPPORT REGISTER ADDRESSING MODE */
    IMM,    /* FIELD SUPPORT IMMEDIATE ADDRESSING MODE */
    BOTH    /* FIELD SUPPORT BOTH ADDRESSING MODES */
} field_adrm_t;

typedef struct trans
{
    const char   str[5];    /* THE MNEMONIC STRING (EX: LDR; ADD; etc.) */
    uint8_t      value;     /* THE TRANSLATED OPCODE OF THE MNEMONIC */
    field_adrm_t dst;       /* INSTRUCTION DESTINATION FIELD */
    field_adrm_t src;       /* INSTRUCTION SOURCE FIELD */
    bool         is_addr;   /* INSTRUCTION NEED ADDRESS (12BITS) NOT DATA (8BITS) */
} trans_t;

const trans_t inst_trans_table[] =
{
    {"HLT" , HLT, NONE, NONE, false},
    {"LDR" , LDR, REG,  BOTH, false},
    {"LDM" , LDM, REG,  BOTH, true},
    {"STI" , STI, REG,  IMM,  true},
    {"STR" , STR, BOTH, REG,  true},
    {"ADD" , ADD, REG,  REG,  false},
    {"SUB" , SUB, REG,  REG,  false},
    {"CMP" , CMP, REG,  REG,  false},
    {"JMP" , JMP, IMM,  NONE, true},
    {"JZ"  , JZ , IMM,  NONE, true},
    {"JNZ" , JNZ, IMM,  NONE, true},
    {"JN"  , JN , IMM,  NONE, true},
    {"JNN" , JNN, IMM,  NONE, true},
    {"JC"  , JC , IMM,  NONE, true},
    {"JNC" , JNC, IMM,  NONE, true},
    {"JBE" , JBE, IMM,  NONE, true},
    {"JA"  , JA , IMM,  NONE, true},
    {"PUSH", PUSH,BOTH, NONE, false},
    {"POP" , POP, REG,  NONE, false},
    {"CALL", CALL,IMM,  NONE, true},
    {"RET" , RET, NONE, NONE, false},
    {"IN"  , IN,  REG,  BOTH, false},
    {"OUT" , OUT, BOTH, REG,  false},
};

const trans_t reg_trans_table[] =
{
    {"R0", 0x0, NONE, NONE, false},
    {"R1", 0x1, NONE, NONE, false},
    {"R2", 0x2, NONE, NONE, false},
    {"R3", 0x3, NONE, NONE, false}
};

#endif