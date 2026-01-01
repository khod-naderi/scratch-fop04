/*
This cpp file is for handling UI component
*/

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "ui.h"
#include "menubar.h"
#include <iostream>
#include "color.h"

/*
    This function render text
*/
SDL_Texture *renderText(SDL_Renderer *renderer, TTF_Font *font, const char *text, SDL_Color color)
{
    SDL_Surface *surface = TTF_RenderText_Blended(font, text, color);
    if (!surface)
        return nullptr;
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

/*
    This function will check that a point is inside a rectangle or not
*/
bool isPointInRect(int x, int y, SDL_Rect rect)
{
    return (x >= rect.x) && (x <= (rect.x + rect.w)) && (y >= rect.y) && (y <= (rect.y + rect.h));
}
