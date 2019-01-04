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

    cpu.m_regShift = 0xFF00;
    cpu.m_regA = 0xBB;

    cpu.m_pc = 0x2000;
    cpu.m_memory[0x2000] = 0xD3;
    cpu.m_memory[0x2001] = 0x4;

    cpu.op_out();

    printf("M Regshift: 0x%X\n", cpu.m_regShift);

    exit(0);

    /* Main loop */
    //for(;;) {
    //    cpu.run_next_op();
    //}

    return 0;
}
