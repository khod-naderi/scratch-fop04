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

void CMD_Move10StepForward();
void CMD_Move10StepBackward();
void CMD_Rotate10DegreeClockwise();
void CMD_Rotate10DegreeCounterClockwise();
void CMD_Sleep2Second();

const Block blocksLibrary[] = {
    Block("On Keyboard Press 'SPACE'", CATEGORY_EVENT, BLOCK_EVENT),
    Block("Move 10 step forward", CATEGORY_CONTROL, BLOCK_COMMAND, CMD_Move10StepForward),
    Block("Move 10 step backward", CATEGORY_CONTROL, BLOCK_COMMAND, CMD_Move10StepBackward),
    Block("Rotate 10 degree clockwise", CATEGORY_CONTROL, BLOCK_COMMAND, CMD_Move10StepBackward),
    Block("Rotate 10 degree counter clockwise", CATEGORY_CONTROL, BLOCK_COMMAND, CMD_Rotate10DegreeCounterClockwise),
    Block("Sleep 2 second", CATEGORY_CONTROL, BLOCK_COMMAND, CMD_Sleep2Second),
};

void drawBlockColumn(SDL_Renderer *renderer, TTF_Font *font, const int mouseX, const int mouseY)
{
    // Draw background of Block column with it's border
    SDL_SetRenderDrawColor(renderer, color_blockColumneBackground);
    SDL_RenderFillRect(renderer, &BLOCKS_COLUMN);
    SDL_SetRenderDrawColor(renderer, color_black);
    SDL_RenderDrawRect(renderer, &BLOCKS_COLUMN);
}