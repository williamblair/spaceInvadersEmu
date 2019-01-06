/*
 * Display.h
 */

#include <cstdio>
#include <SDL/SDL.h>

#ifndef DISPLAY_H_INCLUDED
#define DISPLAY_H_INCLUDED

class Display {

public:

    Display(void);
    virtual ~Display(void);

    bool init(const int w, const int h, const char *title, uint8_t *memory);

    void update(void);

private:

    uint8_t *m_memory; // pointer to board memory


    SDL_Surface *m_space_image; // the space invaders screen

    const int m_width  = 256;
    const int m_height = 224;

    SDL_Surface *m_screen; // SDL display
    SDL_Rect m_screen_rect = { 0, 0, m_width, m_height };

    const int m_vid_start = 0x2400;
    const int m_vid_end   = 0x4000;

    void set_pixels(void);
};


#endif // DISPLAY_H_INCLUDED


