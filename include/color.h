/*
This Header file is for handling color styles
*/

#ifndef COLOR_HEADER
#define COLOR_HEADER

#include <SDL2/SDL.h>

// Wrapper function for giving SDL_Color to SDL_SetRenderDrawColor
int SDL_SetRenderDrawColor(SDL_Renderer *renderer, SDL_Color color)
{
    return SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
}

const SDL_Color color_white = {255, 255, 255, 255};
const SDL_Color color_blue = {0, 0, 255, 255};
const SDL_Color color_black = {0, 0, 0, 0};

const SDL_Color color_menubarBackground = {55, 100, 255, 255};
const SDL_Color color_hoveredMenuButton = {50, 50, 200, 255};
const SDL_Color color_hoveredMenuOption = {200, 200, 200, 255};
const SDL_Color color_menuDropbox = {240, 240, 240, 255};
// const SDL_Color color_menuDropbox = color_menubarBackground;
const SDL_Color color_borderMenuDropbox = {150, 150, 150, 255};

#endif