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
#include <stdint.h>
#include <stdbool.h>
#include "../femto.h"
#include "io.h"


typedef uint8_t (*InFunc)(void);
typedef void    (*OutFunc)(uint8_t data);

InFunc  InputFunction[256]  = {NULL};
OutFunc OutputFunction[256] = {NULL};


void RegisterInputFunc(void *func, uint8_t io_port)
{
    /* VERIFY IF ANOTHER FUNCTION ISN'T ALREADY REGISTER FOR THIS SPECIFIC PORT */
    if (InputFunction[io_port] == NULL)
    {
        InputFunction[io_port] = func;
        // DEBUG:
        printf("DEBUG ==> RegisterInputFunc(): REGISTER func %p to INPUT PORT 0x%02X\n", func, io_port);
    }
    else
    {
        printf("ERROR: CAN'T REGISTER INPUT FUNCTION %p ==> HARDWARE CONFLICT FOR INPUT PORT 0x%02X !!!\n", func, io_port);
    }
}


void RegisterOutputFunc(void *func, uint8_t io_port)
{
    /* VERIFY IF ANOTHER FUNCTION ISN'T ALREADY REGISTER FOR THIS SPECIFIC PORT */
    if (OutputFunction[io_port] == NULL)
    {
        OutputFunction[io_port] = func;
        // DEBUG:
        printf("DEBUG ==> RegisterOutputFunc(): REGISTER func %p to OUTPUT PORT 0x%02X\n", func, io_port);
    }
    else
    {
        printf("ERROR: CAN'T REGISTER OUTPUT FUNCTION %p ==> HARDWARE CONFLICT FOR OUTPUT PORT 0x%02X !!!\n", func, io_port);
    }
}


uint8_t In(uint8_t io_port)
{
    return (*InputFunction[io_port])();
}


void Out(uint8_t data, uint8_t io_port)
{
    (*OutputFunction[io_port])(data);
}