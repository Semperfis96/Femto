#ifndef COMMON_H_
#define COMMON_H_

#include <stdint.h>
#include <stdbool.h>


#define FEMTO_VERSION "1.0.0"
#define ASM_VERSION   "1.0.0"
#define DISM_VERSION  "1.0.0"

#define CFLAG ((flags >>  1) & 0x1)
#define ZFLAG  (flags & 0x1)
#define NFLAG ((flags >>  2) & 0x1)

#define ADRM_IMM  false
#define ADRM_REG  true


typedef enum inst
{
    HLT = 0x0,
    LDR = 0x1,
    LDM = 0x2,
    STI = 0x3,
    STR = 0x4,
    ADD = 0x5,
    SUB = 0x6,
    CMP = 0x7,
    JZ  = 0x8, /* JUMP IF ZERO; SAME AS JE "x == y"*/
    JN  = 0x9, /* JUMP IF NEGATIVE "x < 0" */
    JC  = 0xA, /* JUMP IF CARRY; SAME AS JB (JUMP IF BELOW) "x < y" */
    JNC = 0xB, /* JUMP IF NOT CARRY; SAME AS JAE (JUMP IF ABOVE OR EQUAL) " x >= y"*/
    JBE = 0xC, /* JUMP IF BELOW OR EQUAL (CARRY OR ZERO) "x <= y */
    JA  = 0xD, /* JUMP IF ABOVE (!CARRY OR !ZERO) "x > y" */
    JMP = 0xE
} inst_t;


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
        const char   str[4];    /* THE MNEMONIC STRING (EX: LDR; ADD; etc.) */
        uint8_t      value;     /* THE TRANSLATED OPCODE OF THE MNEMONIC */
        field_adrm_t dst;       /* INSTRUCTION DESTINATION FIELD */
        field_adrm_t src;       /* INSTRUCTION SOURCE FIELD */
        bool         is_addr;   /* INSTRUCTION NEED ADDRESS (12BITS) NOT DATA (8BITS) */
    } trans_t;

    const trans_t inst_trans_table[] =
    {
        {"HLT", HLT, NONE, NONE, false},
        {"LDR", LDR, REG,  BOTH, false},
        {"LDM", LDM, REG,  BOTH, true},
        {"STI", STI, REG,  IMM,  true},
        {"STR", STR, BOTH, REG,  true},
        {"ADD", ADD, REG,  REG,  false},
        {"SUB", SUB, REG,  REG,  false},
        {"CMP", CMP, REG,  REG,  false},
        {"JMP", JMP, IMM,  NONE, true},
        {"JZ",  JZ , IMM,  NONE, true},
        {"JN",  JN , IMM,  NONE, true},
        {"JC",  JC , IMM,  NONE, true},
        {"JNC", JNC, IMM,  NONE, true},
        {"JBE", JBE, IMM,  NONE, true},
        {"JA",  JA , IMM,  NONE, true}
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