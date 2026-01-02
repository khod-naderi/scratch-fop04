/*
This cpp file is for handling command Blocks
And Ui part of Blocks list
*/

#include "block.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "color.h"
#include "ui.h"
#include "generaldef.h"

const Block blocksLibrary[] = {};

void drawBlockColumn(SDL_Renderer *renderer, TTF_Font *font, const int mouseX, const int mouseY)
{
    // Draw background of Block column with it's border
    SDL_SetRenderDrawColor(renderer, color_blockColumneBackground);
    SDL_RenderFillRect(renderer, &BLOCKS_COLUMN);
    SDL_SetRenderDrawColor(renderer, color_black);
    SDL_RenderDrawRect(renderer, &BLOCKS_COLUMN);
}