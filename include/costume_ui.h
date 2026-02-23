#ifndef COSTUME_UI_H
#define COSTUME_UI_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

void drawCostumeEditor(SDL_Renderer *renderer,
                       TTF_Font *font,
                       int mouseX,
                       int mouseY);

#endif
