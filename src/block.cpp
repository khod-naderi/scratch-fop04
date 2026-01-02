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
#include <iostream>

int scrollIndex = 0;

void CMD_Move10StepForward()
{
}

void CMD_Move10StepBackward()
{
}

void CMD_Rotate10DegreeClockwise()
{
}

void CMD_Rotate10DegreeCounterClockwise()
{
}

void CMD_Sleep2Second()
{
}

int blocks_count = 50;

Block blocksLibrary[] = {
    Block("On Keyboard Press 'SPACE'", CATEGORY_EVENT, BLOCK_EVENT),
    Block("Move 10 step forward", CATEGORY_CONTROL, BLOCK_COMMAND, CMD_Move10StepForward),
    Block("Move 10 step backward", CATEGORY_CONTROL, BLOCK_COMMAND, CMD_Move10StepBackward),
    Block("Rotate 10 degree clockwise", CATEGORY_CONTROL, BLOCK_COMMAND, CMD_Move10StepBackward),
    Block("Rotate 10 degree counter clockwise", CATEGORY_CONTROL, BLOCK_COMMAND, CMD_Rotate10DegreeCounterClockwise),
    Block("Sleep 2 second", CATEGORY_CONTROL, BLOCK_COMMAND, CMD_Sleep2Second),
    Block("Move 10 step forward", CATEGORY_CONTROL, BLOCK_COMMAND, CMD_Move10StepForward),
    Block("Move 10 step forward", CATEGORY_CONTROL, BLOCK_COMMAND, CMD_Move10StepForward),
    Block("Move 10 step forward", CATEGORY_CONTROL, BLOCK_COMMAND, CMD_Move10StepForward),
    Block("Move 10 step forward", CATEGORY_CONTROL, BLOCK_COMMAND, CMD_Move10StepForward),
    Block("Move 10 step forward", CATEGORY_CONTROL, BLOCK_COMMAND, CMD_Move10StepForward),
    Block("Move 10 step forward", CATEGORY_CONTROL, BLOCK_COMMAND, CMD_Move10StepForward),
    Block("Move 10 step forward", CATEGORY_CONTROL, BLOCK_COMMAND, CMD_Move10StepForward),
    Block("Move 10 step forward", CATEGORY_CONTROL, BLOCK_COMMAND, CMD_Move10StepForward),
    Block("Move 10 step forward", CATEGORY_CONTROL, BLOCK_COMMAND, CMD_Move10StepForward),
    Block("Move 10 step forward", CATEGORY_CONTROL, BLOCK_COMMAND, CMD_Move10StepForward),
    Block("Move 10 step forward", CATEGORY_CONTROL, BLOCK_COMMAND, CMD_Move10StepForward),
    Block("Move 10 step forward", CATEGORY_CONTROL, BLOCK_COMMAND, CMD_Move10StepForward),
    Block("Move 10 step forward", CATEGORY_CONTROL, BLOCK_COMMAND, CMD_Move10StepForward),
    Block("Move 10 step forward", CATEGORY_CONTROL, BLOCK_COMMAND, CMD_Move10StepForward),
    Block("Move 10 step forward", CATEGORY_CONTROL, BLOCK_COMMAND, CMD_Move10StepForward),
    Block("Move 10 step forward", CATEGORY_CONTROL, BLOCK_COMMAND, CMD_Move10StepForward),
    Block("Move 10 step forward", CATEGORY_CONTROL, BLOCK_COMMAND, CMD_Move10StepForward),
    Block("Move 10 step forward", CATEGORY_CONTROL, BLOCK_COMMAND, CMD_Move10StepForward),
    Block("Move 10 step forward", CATEGORY_CONTROL, BLOCK_COMMAND, CMD_Move10StepForward),
    Block("Move 10 step forward", CATEGORY_CONTROL, BLOCK_COMMAND, CMD_Move10StepForward),
    Block("Move 10 step forward", CATEGORY_CONTROL, BLOCK_COMMAND, CMD_Move10StepForward),
    Block("Move 10 step forward", CATEGORY_CONTROL, BLOCK_COMMAND, CMD_Move10StepForward),
    Block("Move 10 step forward", CATEGORY_CONTROL, BLOCK_COMMAND, CMD_Move10StepForward),
    Block("Move 10 step forward", CATEGORY_CONTROL, BLOCK_COMMAND, CMD_Move10StepForward),
    Block("Move 10 step forward", CATEGORY_CONTROL, BLOCK_COMMAND, CMD_Move10StepForward),
    Block("Move 10 step forward", CATEGORY_CONTROL, BLOCK_COMMAND, CMD_Move10StepForward),
    Block("Move 10 step forward", CATEGORY_CONTROL, BLOCK_COMMAND, CMD_Move10StepForward),
    Block("Move 10 step forward", CATEGORY_CONTROL, BLOCK_COMMAND, CMD_Move10StepForward),
    Block("Move 10 step forward", CATEGORY_CONTROL, BLOCK_COMMAND, CMD_Move10StepForward),
    Block("Move 10 step forward", CATEGORY_CONTROL, BLOCK_COMMAND, CMD_Move10StepForward),
    Block("Move 10 step forward", CATEGORY_CONTROL, BLOCK_COMMAND, CMD_Move10StepForward),
    Block("Move 10 step forward", CATEGORY_CONTROL, BLOCK_COMMAND, CMD_Move10StepForward),
    Block("Move 10 step forward", CATEGORY_CONTROL, BLOCK_COMMAND, CMD_Move10StepForward),
    Block("Move 10 step forward", CATEGORY_CONTROL, BLOCK_COMMAND, CMD_Move10StepForward),
    Block("Move 10 step forward", CATEGORY_CONTROL, BLOCK_COMMAND, CMD_Move10StepForward),
    Block("Move 10 step forward", CATEGORY_CONTROL, BLOCK_COMMAND, CMD_Move10StepForward),
    Block("Move 10 step forward", CATEGORY_CONTROL, BLOCK_COMMAND, CMD_Move10StepForward),
    Block("Move 10 step forward", CATEGORY_CONTROL, BLOCK_COMMAND, CMD_Move10StepForward),
    Block("Move 10 step forward", CATEGORY_CONTROL, BLOCK_COMMAND, CMD_Move10StepForward),
    Block("Move 10 step forward", CATEGORY_CONTROL, BLOCK_COMMAND, CMD_Move10StepForward),
    Block("Move 10 step forward", CATEGORY_CONTROL, BLOCK_COMMAND, CMD_Move10StepForward),
    Block("Move 10 step forward", CATEGORY_CONTROL, BLOCK_COMMAND, CMD_Move10StepForward),
    Block("Move 10 step forward", CATEGORY_CONTROL, BLOCK_COMMAND, CMD_Move10StepForward),
    Block("Move 10 step forward", CATEGORY_CONTROL, BLOCK_COMMAND, CMD_Move10StepForward),
    Block("Move 10 step forward", CATEGORY_CONTROL, BLOCK_COMMAND, CMD_Move10StepForward),

};

int blockColumnInit(SDL_Renderer *renderer, TTF_Font *font)
{
    // Prerender every command texture and bound
    for (int i = 0; i < blocks_count; i++)
    {
        blocksLibrary[i].textrue = renderText(renderer, font, blocksLibrary[i].label, color_white);
        SDL_QueryTexture(blocksLibrary[i].textrue, NULL, NULL, &blocksLibrary[i].width, &blocksLibrary[i].height);
        blocksLibrary[i].width += 10;
        blocksLibrary[i].height += 5;
    }
    return 0;
}

void drawBlockColumn(SDL_Renderer *renderer, TTF_Font *font, const int mouseX, const int mouseY)
{
    // Draw background of Block column with it's border
    SDL_SetRenderDrawColor(renderer, color_blockColumneBackground);
    SDL_RenderFillRect(renderer, &BLOCKS_COLUMN);
    SDL_SetRenderDrawColor(renderer, color_black);
    SDL_RenderDrawRect(renderer, &BLOCKS_COLUMN);

    // Draw Blocks in Block Column
    int block_count = 0, visibleBlockCount = 0;
    int blocksTotalHeight = 5;
    for (int i = 0; i < blocks_count; i++)
    {
        if (blocksLibrary[i].categoryId == selectedCategoryId)
        {
            SDL_Rect itemRect = {
                BLOCKS_COLUMN.x + 5,
                BLOCKS_COLUMN.y + blocksTotalHeight - scrollIndex,
                blocksLibrary[i].width,
                blocksLibrary[i].height,
            };
            blocksTotalHeight += blocksLibrary[i].height + 5;
            block_count++;

            // skip of not in the scrolled section.
            if ((blocksTotalHeight - scrollIndex < 0) || (blocksTotalHeight - scrollIndex > MAIN_WINDOW_HEIGHT))
            {
                continue;
            }
            visibleBlockCount++;

            // Background
            if (isPointInRect(mouseX, mouseY, itemRect))
            {
                SDL_SetRenderDrawColor(renderer, colorDim(categories[blocksLibrary[i].categoryId].color));
            }
            else
            {
                SDL_SetRenderDrawColor(renderer, categories[blocksLibrary[i].categoryId].color);
            }
            SDL_RenderFillRect(renderer, &itemRect);

            // border
            SDL_SetRenderDrawColor(renderer, color_black);
            SDL_RenderDrawRect(renderer, &itemRect);

            // text
            int tw, th;
            SDL_QueryTexture(blocksLibrary[i].textrue, NULL, NULL, &tw, &th);
            SDL_Rect textRect = {
                itemRect.x + (itemRect.w - tw) / 2,
                itemRect.y + (itemRect.h - th) / 2,
                tw,
                th,
            };
            SDL_RenderCopy(renderer, blocksLibrary[i].textrue, NULL, &textRect);
        }
    }
}

void controlBlockColumnMouseScroll(const int mouseX, const int mouseY, const Sint32 scrollY)
{
    if (isPointInRect(mouseX, mouseY, BLOCKS_COLUMN))
    {
        std::cout << scrollY << std::endl;
        scrollIndex += scrollY * SCROLL_Y_SENSIVITY;

        if (scrollIndex < 0)
            scrollIndex = 0;
    }
}