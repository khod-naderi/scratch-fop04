
#include <SDL2/SDL.h>
/*
This header file is for handling command Blocks
*/

#ifndef BLOCK_HEADER
#define BLOCK_HEADER

struct Category
{
    const char *name;
    SDL_Color color;
    SDL_Rect bounds;
    bool isSelected;
};

struct Block
{
    const char *label;
    unsigned int categoryId;
    SDL_Color color;
    SDL_Rect bounds;
};

#endif