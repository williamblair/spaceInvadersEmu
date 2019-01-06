/*
 * Display.cpp
 */

#include "Display.h"

Display::Display(void)
{
    m_memory = NULL;
    m_screen = NULL;
    m_space_image = NULL;
}

Display::~Display(void)
{
    if (m_space_image) {
        SDL_FreeSurface(m_space_image);
    }

    if (m_screen) {
        SDL_FreeSurface(m_screen);
        SDL_Quit();
    }
}

bool Display::init(const int w, const int h, const char *title, Cpu8080 *cpu)
{
    // 256x224

    SDL_Init(SDL_INIT_VIDEO);

    m_screen = SDL_SetVideoMode(w, h, 32, SDL_HWSURFACE);

    m_cpu = cpu;

    m_memory = cpu->m_memory;


    m_space_image = SDL_CreateRGBSurface(
            0,
            m_width,
            m_height,
            32,
            0xFF000000, // Rmask
            0x00FF0000, // Gmask
            0x0000FF00, // Bmask
            0x000000FF  // Alpha
        );

    return true;
}

void Display::update(void)
{
    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT) {
            exit(0);
        }

        if (event.type == SDL_KEYDOWN) {

            switch (event.key.keysym.sym)
            {
                case SDLK_RETURN:
                    printf("  Key Enter Down\n");
                    m_cpu->m_port1 |= 0b00000100; // Set bit 2 or port 1 (1P Start)
                    break;
                case SDLK_LEFT:
                    printf("  Key Left Down\n");
                    m_cpu->m_port1 |= 0b00100000; // Set bit 5 of port 1 (1P Left)
                    break;
                case SDLK_RIGHT:
                    printf("  Key Right Down\n");
                    m_cpu->m_port1 |= 0b01000000; // Set bit 6 of port 1 (1P Right)
                    break;
                case SDLK_SPACE:
                    printf("  Key Space Down\n");
                    m_cpu->m_port1 |= 0b00010000; // Set bit 4 of port 1 (1P shoot)
                default:
                    break;
            }

        } // if (SDL_KEYDOWN)

        else if (event.type == SDL_KEYUP) {

            switch (event.key.keysym.sym)
            {
                case SDLK_RETURN:
                    printf("  Key Enter Up\n");
                    m_cpu->m_port1 &= ~0b00000100; // Clear bit 2 or port 1 (1P Start)
                    break;
                case SDLK_LEFT:
                    printf("  Key Left Up\n");
                    m_cpu->m_port1 &= ~0b00100000; // Clear bit 5 of port 1 (1P Left)
                    break;
                case SDLK_RIGHT:
                    printf("  Key Right Up\n");
                    m_cpu->m_port1 &= ~0b01000000; // Clear bit 6 of port 1 (1P Right)
                    break;
                case SDLK_SPACE:
                    printf("  Key Space Up\n");
                    m_cpu->m_port1 &= ~0b00010000; // Clear bit 4 of port 1 (1P shoot)
                default:
                    break;
            }

        } // else if (SDL_KEYUP)
    }

    SDL_Delay(1000.0f/30.0f);

    set_pixels();

    SDL_BlitSurface(m_space_image, NULL, m_screen, &m_screen_rect);
    SDL_Flip(m_screen);
}

void Display::set_pixels(void)
{
    int i, j;
    int pixel_index = 0;

    SDL_LockSurface(m_space_image);

    for (i = m_vid_start; i < m_vid_end; ++i)
    {
        /* Get the current byte */
        uint8_t cur = m_memory[i];

        /* Test each bit */
        for (j = 0; j < 8; ++j) {
            ((Uint32 *)m_space_image->pixels)[(pixel_index++)] = 
                (cur & 1) ? 0xFFFFFFFF : 0x0;

            cur >>= 1;
        }
    }

    SDL_UnlockSurface(m_space_image);
}

