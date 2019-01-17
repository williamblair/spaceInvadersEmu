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

    /* Set some initial input values 
     * Port 2
     * bit 0 = DIP3 00 = 3 ships  10 = 5 ships
     * bit 1 = DIP5 01 = 4 ships  11 = 6 ships
     * bit 2 = Tilt
     * bit 3 = DIP6 0 = extra ship at 1500, 1 = extra ship at 1000
     * bit 4 = P2 shot (1 if pressed)
     * bit 5 = P2 left (1 if pressed)
     * bit 6 = P2 right (1 if pressed)
     * bit 7 = DIP7 Coin info displayed in demo screen 0=ON
     */
    ports[2] = 0;

    /* Port 1
     *  bit 0 = CREDIT (1 if deposit)
     *  bit 1 = 2P start (1 if pressed)
     *  bit 2 = 1P start (1 if pressed)
     *  bit 3 = Always 1
     *  bit 4 = 1P shot (1 if pressed)
     *  bit 5 = 1P left (1 if pressed)
     *  bit 6 = 1P right (1 if pressed)
     *  bit 7 = Not connected
     */
    ports[1] = 0b00001000;

#if 0
    for (i=0; i < 0x2000; ++i) {
        printf("%02X ", cpu.m_memory[i]);

        if (i!=0 && (i+1)%8 == 0) printf("\n");
    }
    exit(0);
#endif

    display.init(256, 224, memory, ports);
    cpu.init(memory, ports);

    timer = SDL_GetTicks();

    /* Main loop */
    for(;;) 
    {
// just to speed things up for implementation
        if (SDL_GetTicks() - timer > (1000/60)) {

            cpu.run_interrupt(2);


            timer = SDL_GetTicks();
        }

        display.update();
        cpu.run_next_op();

    }

    return 0;
}



