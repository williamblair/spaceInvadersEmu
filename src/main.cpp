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

#if 0
    for (i=0; i < 0x2000; ++i) {
        printf("%02X ", cpu.m_memory[i]);

        if (i!=0 && (i+1)%8 == 0) printf("\n");
    }
    exit(0);
#endif

    display.init(224, 256, "Hello World", &cpu);

    timer = SDL_GetTicks();

    /* Main loop */
    for(;;) 
    {
        if (SDL_GetTicks() - timer > (1000/60)) {
            timer = SDL_GetTicks();

            display.update();
            cpu.run_next_op();
        }

    }

    return 0;
}
