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

#ifndef TEST_H_
#define TEST_H_

#define ERROR(s)    printf("!!! ERROR : %s TEST NOT PASSED !!!\n", s); \
                    exit(-1);

#define PASSED(s)   printf("==> %s TEST PASSED\n", s);

#define ASSERT_EQ(a, b, s)  if (a == b)                                         \
                            {                                                   \
                                PASSED(s)                                       \
                            }                                                   \
                            else                                                \
                            {                                                   \
                                printf("!!! ASSERT_EQ FAILED FOR %s !!!\n", s); \
                                ERROR(s)                                        \
                            }

#endif