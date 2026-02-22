/*
-------------------------------------------
This header file is for managing stage view named canvas
-------------------------------------------
*/

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "spritebox.h"
#include <vector>

#ifndef CANVAS_HEADER
#define CANVAS_HEADER

// This will hold live sprints
extern std::vector<SprintBody> aliveSprints;

void drawCanvasScreen(SDL_Renderer *renderer, TTF_Font *font, const int mouseX, const int mouseY);
void setBackground(SDL_Renderer* renderer, const std::string& imagePath);
#endif