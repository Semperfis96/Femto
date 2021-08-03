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

#ifndef TEST_H_
#define TEST_H_

#define ERROR(s)    printf("!!! ERROR : %s TEST NOT PASSED !!!\n", s); \
                    exit(-1);

#define ASSERT_EQ(a, b, s)  if (a == b)                                         \
                            {                                                   \
                                printf("==> %s TEST PASSED\n", s);              \
                            }                                                   \
                            else                                                \
                            {                                                   \
                                printf("!!! ASSERT_EQ FAILED FOR %s !!!\n", s); \
                                ERROR(s)                                        \
                            }


/*** REFERENCE TO CPU.C FUNCTION THAT AREN'T EXPLICITLY EXPORTED IN A HEADER FILE, BUT LINK AT COMPILE TIME WITH CPU.O ***/
void    StackPushByte(FemtoEmu_t *emu, uint8_t byte);
uint8_t StackPopByte(FemtoEmu_t *emu);
void    OpcodeHlt(FemtoEmu_t *emu, bool verbose);
void    OpcodeLdr(FemtoEmu_t *emu, bool verbose);
void    OpcodeLdm(FemtoEmu_t *emu, bool verbose);
void    OpcodeSti(FemtoEmu_t *emu, bool verbose);
void    OpcodeStr(FemtoEmu_t *emu, bool verbose);
void    OpcodeAdd(FemtoEmu_t *emu, bool verbose);
void    OpcodeSub(FemtoEmu_t *emu, bool verbose);
void    OpcodeCmp(FemtoEmu_t *emu, bool verbose);
void    OpcodeJz(FemtoEmu_t *emu, bool verbose);
void    OpcodeJn(FemtoEmu_t *emu, bool verbose);
void    OpcodeJc(FemtoEmu_t *emu, bool verbose);
void    OpcodeJnc(FemtoEmu_t *emu, bool verbose);
void    OpcodeJbe(FemtoEmu_t *emu, bool verbose);
void    OpcodeJa(FemtoEmu_t *emu, bool verbose);
void    OpcodeJmp(FemtoEmu_t *emu, bool verbose);
void    OpcodeJnz(FemtoEmu_t *emu, bool verbose);
void    OpcodeJnn(FemtoEmu_t *emu, bool verbose);
void    OpcodePush(FemtoEmu_t *emu, bool verbose);
void    OpcodePop(FemtoEmu_t *emu, bool verbose);
void    OpcodeCall(FemtoEmu_t *emu, bool verbose);
void    OpcodeRet(FemtoEmu_t *emu, bool verbose);
void    OpcodeSys(FemtoEmu_t *emu, bool verbose);
void    OpcodeSei(FemtoEmu_t *emu, bool verbose);
void    OpcodeSdi(FemtoEmu_t *emu, bool verbose);
void    OpcodeError(FemtoEmu_t *emu, bool verbose);

#endif