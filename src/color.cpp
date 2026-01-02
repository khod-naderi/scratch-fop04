#include <SDL2/SDL.h>
#include "color.h"

// Define color constants
const SDL_Color color_white = {255, 255, 255, 255};
const SDL_Color color_blue = {0, 0, 255, 255};
const SDL_Color color_black = {0, 0, 0, 0};
const SDL_Color color_menubarBackground = {55, 100, 255, 255};
const SDL_Color color_hoveredMenuButton = {50, 50, 200, 255};
const SDL_Color color_hoveredMenuOption = {200, 200, 200, 255};
const SDL_Color color_menuDropbox = {240, 240, 240, 255};
const SDL_Color color_borderMenuDropbox = {150, 150, 150, 255};

extern const SDL_Color colort_eventBlocks = {43, 255, 255, 255};
extern const SDL_Color colort_controlBlocks = {255, 178, 44, 255};
extern const SDL_Color colort_motionBlocks = {160, 255, 88, 255};
extern const SDL_Color colort_soundBlocks = {143, 52, 255, 255};

const SDL_Color color_categoryColumneBackground = {142, 142, 142, 255};

#define COLOR_DIM_SCALE 30

SDL_Color colorDim(SDL_Color color)
{
    SDL_Color color_new;

    if (color.r < COLOR_DIM_SCALE)
        color_new.r = 0;
    else
        color_new.r = (color.r - COLOR_DIM_SCALE);

    if (color.b < COLOR_DIM_SCALE)
        color_new.b = 0;
    else
        color_new.b = (color.b - COLOR_DIM_SCALE);

    if (color.g < COLOR_DIM_SCALE)
        color_new.g = 0;
    else
        color_new.g = (color.g - COLOR_DIM_SCALE);

    color_new.a = color.a;

    return color_new;
}