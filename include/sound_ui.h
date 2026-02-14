#ifndef SOUND_UI_H
#define SOUND_UI_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

// Draw Sound tab workspace
void drawSoundEditor(
    SDL_Renderer *renderer,
    TTF_Font *font,
    int mouseX,
    int mouseY);

#endif
