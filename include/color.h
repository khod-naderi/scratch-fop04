/*
This Header file is for handling color styles
*/

#ifndef COLOR_HEADER
#define COLOR_HEADER

#include <SDL2/SDL.h>

// Wrapper function for giving SDL_Color to SDL_SetRenderDrawColor
inline int SDL_SetRenderDrawColor(SDL_Renderer *renderer, SDL_Color color)
{
    return SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
}

extern const SDL_Color color_white;
extern const SDL_Color color_blue;
extern const SDL_Color color_black;
extern const SDL_Color color_softGray;
extern const SDL_Color color_blurgray;

extern const SDL_Color color_menubarBackground;
extern const SDL_Color color_hoveredMenuButton;
extern const SDL_Color color_hoveredMenuOption;
extern const SDL_Color color_menuDropbox;
// const SDL_Color color_menuDropbox = color_menubarBackground;
extern const SDL_Color color_borderMenuDropbox;

extern const SDL_Color colort_eventBlocks;
extern const SDL_Color colort_controlBlocks;
extern const SDL_Color colort_motionBlocks;
extern const SDL_Color colort_soundBlocks;

extern const SDL_Color color_categoryColumneBackground;
extern const SDL_Color color_addExtentionBtn;

extern const SDL_Color color_blockColumneBackground;

extern const SDL_Color color_spriteBoxBackground;

SDL_Color colorDim(SDL_Color color);
SDL_Color colorLight(SDL_Color color);

#endif