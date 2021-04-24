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
#include <string.h>
#include <stdbool.h>
#include "femto.h"
#include "common.h"


/*** CMD FUNCTIONS ***/
void CmdHelp(void)
{
    printf("Usage: femto [OPTION]\n");
    printf("Run the computer emulator FEMTO with FILE as input binary, configure with OPTION.\n\n");
    printf("--help        : display this help & exit\n");
    printf(" -h | -?\n");
    printf("--version     : output version information and exit\n");
    printf(" -v\n");
    printf("--file [FILE] : specify the binary file to be load in RAM\n");
    printf(" -f\n");
    printf("--verbose     : specify to femto to output more information\n");
    printf(" -vb\n");
}

void CmdVersion(void)
{
    printf("femto %s | Copyright (C) 2021 Semperfis\n", (char *)FEMTO_VERSION);
    printf("This program comes with ABSOLUTELY NO WARRANTY;\n");
    printf("License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>;\n");
    printf("This is free software, and you are welcome to redistribute it under certain conditions;\n");
}
/*** CMD FUNCTIONS END ***/


int main(int argc, char *argv[])
{
    char       *rom      = NULL;
    FemtoEmu_t *EmuState = NULL;
    bool        verbose  = false;


    /*** COMMAND-LINE ARGUMENTS ***/
    if (argc == 1)
    {
        CmdHelp();
        return 0;
    }

    for (int i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "-?") == 0)
        {
            CmdHelp();
            return 0;
        }
        else if (strcmp(argv[i], "--version") == 0 || strcmp(argv[i], "-v") == 0)
        {
            CmdVersion();
            return 0;
        }
        else if (strcmp(argv[i], "--file") == 0 || strcmp(argv[i], "-f") == 0)
        {
            i++;
            rom = argv[i];
        }
        else if (strcmp(argv[i], "--verbose") == 0 || strcmp(argv[i], "-vb") == 0)
        {
            verbose = true;
        }
    }


    /* Start the emulation */
    EmuState = EmuInit(rom, verbose);
    EmuLoop(EmuState, verbose);

    /* End the simulation */
    EmuQuit(EmuState);

    return 0;
}