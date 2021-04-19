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
#define CFLAG ((flags >>  1) & 0x1)
#define ZFLAG  (flags & 0x1)
#define NFLAG ((flags >>  2) & 0x1)

#define ADRM_IMM  false
#define ADRM_REG  true

#define CHKVB (verbose == true)

/*** CONFIGURATION VARIABLES ***/

bool verbose = false;   /* BY DEFAULT, FEMTO ISN'T VERBOSE */

/*** END OF CONFIGURATION VARIABLES ***/


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
} inst_t;


#ifdef _EMU_
    /* TODO: Make a nice structure */
    uint16_t  pc    =  0x0;      /* PROGRAM COUNTER (12BITS) */
    uint8_t   r[4]  = {0x0};     /* GP REGISTERS (R0, R1, R2 AND R3) */
    uint8_t   sp    = 0x00;      /* STACK POINTER */
    uint8_t   flags =  0x0;      /* FLAGS REGISTER */
    uint8_t   f[3]  = {0x0};     /* ARRAY OF BINARY FETCH INSTRUCTION (3 BYTES LONG) */
    uint8_t   inst  = 0;         /* INSTRUCTION CODE */
    uint8_t  *ram   = NULL;      /* VIRTUAL COMPUTER RAM, 4KBs (0x000 - 0xFFF) */
    bool      adrm  = ADRM_IMM;  /* ADDRESSING MODE */
    bool      halt  = false;     /* CPU IS HALT OR NOT */
    uint8_t   data  = 0;         /* 8BITS DATA */
    uint16_t  addr  = 0;         /* 12BITS ADDRESS */
    uint8_t   dreg  = 0;         /* DESTINATION REGISTER */
    uint8_t   sreg  = 0;         /* SOURCE REGISTER */
    int       temp  = 0;
#endif


#ifdef _ASM_
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
        {"POP" , POP, REG,  NONE, false}
    };

    const trans_t reg_trans_table[] =
    {
        {"R0", 0x0, NONE, NONE, false},
        {"R1", 0x1, NONE, NONE, false},
        {"R2", 0x2, NONE, NONE, false},
        {"R3", 0x3, NONE, NONE, false}
    };
#endif
#endif