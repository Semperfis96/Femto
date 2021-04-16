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

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "opcode.h"

#ifdef _ASM_
    #undef _ASM_
#endif

#ifndef _EMU_
#define _EMU_
    #include "common.h"
#endif



/*** CMD FUNCTIONS ***/
void cmd_help(void)
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

void cmd_version(void)
{
    printf("femto %s | Copyright (C) 2021 Semperfis\n", (char *)FEMTO_VERSION);
    printf("This program comes with ABSOLUTELY NO WARRANTY;\n");
    printf("License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>;\n");
    printf("This is free software, and you are welcome to redistribute it under certain conditions;\n");
}
/*** CMD FUNCTIONS END ***/


/*** HELPER FUNCTIONS ***/
int rom_load(const char *rom_file, uint8_t *ram)
{
    FILE *rom  = NULL;
    long  size = 0L;

    rom = fopen(rom_file, "rb");
    if (rom == NULL)
    {
        printf("ERROR (rom_load): CAN'T OPEN FILE \"%s\" !!!\n", rom_file);
        return -1;
    }

    /* GET ROM SIZE */
    fseek(rom, 0L, SEEK_END);
    size = ftell(rom);
    fseek(rom, 0L, SEEK_SET);

    /* READ ROM TO RAM AT 0x000 */
    if (fread((void *)ram, sizeof(uint8_t), (size_t)size, rom) != (size_t)size)
    {
        printf("ERROR (rom_load): CAN'T READ PROPERLY THE FILE \"%s\" !!!\n", rom_file);
        fclose(rom);
        return -1;
    }

    fclose(rom);
    return 0;
}
/*** HELPER FUNCTIONS END ***/


/*** PROGRAM ENTRY POINT ***/
int main(int argc, char *argv[])
{
    char     *rom   = NULL;

    /*** COMMAND-LINE ARGUMENTS ***/
    if (argc == 1)
    {
        cmd_help();
        return 0;
    }

    for (int i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "-?") == 0)
        {
            cmd_help();
            return 0;
        }
        else if (strcmp(argv[i], "--version") == 0 || strcmp(argv[i], "-v") == 0)
        {
            cmd_version();
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


    /*** EMULATION SETUP ***/
    /*   RAM ALLOCATION    */
    ram = malloc(4 * 1024 * sizeof(uint8_t));
    if (ram == NULL)
    {
        printf("ERROR (main): CAN'T ALLOCATE VIRTUAL RAM !!!\n");
        exit(-1);
    }
    if (CHKVB) printf("FEMTO: VIRTUAL RAM IS ALLOCATE\n");

    /* ROM LOADING (BINARY FILE) INTO RAM */
    if (rom_load((const char *)rom, ram) != 0)
    {
        free(ram);
        return -1;
    }
    if (CHKVB) printf("FEMTO: ROM IS LOAD IN VIRTUAL RAM\n");


    /*** EMULATION LOOP ***/
    printf("FEMTO: STARTING EMULATION\n");
    while (!halt)
    {
        /* FETCH INSTRUCTION FROM RAM */
        if (CHKVB) printf("[0x%03X] ", pc);
        f[0] = ram[(pc++ % 0xFFF)];
        f[1] = ram[(pc++ % 0xFFF)];
        f[2] = ram[(pc++ % 0xFFF)];

        /* DECODE INSTRUCTION */
        inst =   f[0] & 0x7F;
        adrm =   f[0] & 0x80;
        dreg =  (f[1] >> 6) & 0x03;
        sreg =  (f[1] >> 4) & 0x03;
        data =   f[2];
        addr = ((f[1] & 0x0F) << 8) | f[2];

        /* EXECUTE INSTRUCTION, CALL THE APPROPRIATE FUNCTION THAT EMULATE THE OPCODE */
        (*OpcodeFunc[inst])();
        temp = 0;
    }

    /*** EMULATION END ***/
    free(ram);
    return 0;
}