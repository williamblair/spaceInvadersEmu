/*
 * main.cpp
 */

#include <cstdio>

#include "Cpu8080.h"
#include "Display.h"

/*
 * Links:
 * http://computerarcheology.com/Arcade/SpaceInvaders/Code.html#addr_01E6 - disassembled space invader code, plus hardware specifications
 * http://altairclone.com/downloads/manuals/8080%20Programmers%20Manual.pdf - intel 8080 assembly manual - describes opcodes
 */

int main(int argc, char *argv[])
{
    Cpu8080 cpu;
    Display display;

    display.init(256, 224, "Hello World", cpu.m_memory);


    /* Main loop */
    for(;;) {
        cpu.run_next_op();

        display.update();
    }

    return 0;
}
