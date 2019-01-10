/*
 * main.cpp
 */

#include <cstdio>
#include <SDL/SDL.h>
#include "Cpu8080.h"
#include "Display.h"

Uint32 timer = 0;

/*
 * Links:
 * http://computerarcheology.com/Arcade/SpaceInvaders/Code.html#addr_01E6 - disassembled space invader code, plus hardware specifications
 * http://altairclone.com/downloads/manuals/8080%20Programmers%20Manual.pdf - intel 8080 assembly manual - describes opcodes
 */

int main(int argc, char *argv[])
{
    int i;

    Cpu8080 cpu;
    Display display;

    uint8_t memory[0x4000]; // System memory
    uint8_t ports[0x7]; // Ports; port 0..6

    /* Init with zeros */
    memset(memory, 0, sizeof(memory));
    memset(ports, 0, sizeof(ports));

    /* Load the ROM into system memory */
    memcpy(&memory[ROM_H_START], ROM_H, ROM_H_SIZE);
    memcpy(&memory[ROM_G_START], ROM_G, ROM_G_SIZE);
    memcpy(&memory[ROM_F_START], ROM_F, ROM_F_SIZE);
    memcpy(&memory[ROM_E_START], ROM_E, ROM_E_SIZE);


#if 0
    for (i=0; i < 0x2000; ++i) {
        printf("%02X ", cpu.m_memory[i]);

        if (i!=0 && (i+1)%8 == 0) printf("\n");
    }
    exit(0);
#endif

    display.init(224, 256, memory, ports);
    cpu.init(memory, ports);

    timer = SDL_GetTicks();

    /* Main loop */
    for(;;) 
    {
// just to speed things up for implementation
        if (SDL_GetTicks() - timer > (1000/60)) {

            cpu.run_interrupt(2);

            display.update();
            cpu.run_next_op();

            timer = SDL_GetTicks();
        }

    }

    return 0;
}



