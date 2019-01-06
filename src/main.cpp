/*
 * main.cpp
 */

#include <cstdio>

#include "Cpu8080.h"

/*
 * Links:
 * http://computerarcheology.com/Arcade/SpaceInvaders/Code.html#addr_01E6 - disassembled space invader code, plus hardware specifications
 * http://altairclone.com/downloads/manuals/8080%20Programmers%20Manual.pdf - intel 8080 assembly manual - describes opcodes
 */

int main(int argc, char *argv[])
{
    Cpu8080 cpu;


    /* Main loop */
    for(;;) {
        cpu.run_next_op();
    }

    return 0;
}
