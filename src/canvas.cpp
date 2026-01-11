/*
-------------------------------------------
This CPP file is for managing stage view named canvas
-------------------------------------------
*/

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "canvas.h"
#include "color.h"
#include "ui.h"

/*
-------------------------------------------------
This function is for drawing Canvas Screen.
this function will run on every frame.
-------------------------------------------------
*/
void drawCanvasScreen(SDL_Renderer *renderer, TTF_Font *font, const int mouseX, const int mouseY)
{
    // Draw border of Canvas
    SDL_SetRenderDrawColor(renderer, color_black);
    SDL_RenderDrawRect(renderer, &CANVAS_BOX);
}