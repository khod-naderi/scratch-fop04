/*
-------------------------------------------
This header file is for managing stage view named canvas
-------------------------------------------
*/

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#ifndef CANVAS_HEADER
#define CANVAS_HEADER

void drawCanvasScreen(SDL_Renderer *renderer, TTF_Font *font, const int mouseX, const int mouseY);
#endif