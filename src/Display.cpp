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

bool Display::init(const int w, const int h, const char *title, uint8_t *memory)
{
    // 256x224

    SDL_Init(SDL_INIT_VIDEO);

    m_screen = SDL_SetVideoMode(w, h, 32, SDL_HWSURFACE);

    m_memory = memory;


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

