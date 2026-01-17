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
#include <vector>

/*
--------------------------------
 Scroll properties
-------------------------------
*/
int scrollBlockColumnIndex = 0;

/*
--------------------------------
 Drag & Drop properties
-------------------------------
*/
bool isBLockDraged = false;
int dragedBlockIndex;

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

std::vector<Block> blocksLibrary = {
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
    for (int i = 0; i < BLOCKS_COUNT; i++)
    {
        blocksLibrary[i].textrue = renderText(renderer, font, blocksLibrary[i].label, color_white);
        SDL_QueryTexture(blocksLibrary[i].textrue, NULL, NULL, &blocksLibrary[i].width, &blocksLibrary[i].height);
        blocksLibrary[i].width += 10;
        blocksLibrary[i].height += 5;
    }
    return 0;
}

int blocksScrollLimit = 0;

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
    for (int i = 0; i < BLOCKS_COUNT; i++)
    {
        if (blocksLibrary[i].categoryId == selectedCategoryId)
        {
            SDL_Rect itemRect = {
                BLOCKS_COLUMN.x + 5,
                BLOCKS_COLUMN.y + blocksTotalHeight - scrollBlockColumnIndex,
                blocksLibrary[i].width,
                blocksLibrary[i].height,
            };
            blocksTotalHeight += blocksLibrary[i].height + 5;
            block_count++;

            // skip of not in the scrolled section.
            if ((blocksTotalHeight - scrollBlockColumnIndex < 0) || (blocksTotalHeight - scrollBlockColumnIndex > MAIN_WINDOW_HEIGHT))
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
    blocksScrollLimit = blocksTotalHeight - scrollBlockColumnIndex - (blocksLibrary[BLOCKS_COUNT - 1].height);

    // Draw draged block
    if (isBLockDraged)
    {
        // Draw draged item centre on mouse pointer
        SDL_Rect dragedItemRect = {
            mouseX - blocksLibrary[dragedBlockIndex].width / 2,
            mouseY - blocksLibrary[dragedBlockIndex].height / 2,
            blocksLibrary[dragedBlockIndex].width,
            blocksLibrary[dragedBlockIndex].height,
        };

        // Fill item background
        SDL_SetRenderDrawColor(renderer, categories[blocksLibrary[dragedBlockIndex].categoryId].color);
        SDL_RenderFillRect(renderer, &dragedItemRect);

        // item border
        SDL_SetRenderDrawColor(renderer, color_black);
        SDL_RenderDrawRect(renderer, &dragedItemRect);

        // item text
        int tw, th;
        SDL_QueryTexture(blocksLibrary[dragedBlockIndex].textrue, NULL, NULL, &tw, &th);
        SDL_Rect textRect = {
            dragedItemRect.x + (dragedItemRect.w - tw) / 2,
            dragedItemRect.y + (dragedItemRect.h - th) / 2,
            tw,
            th,
        };
        SDL_RenderCopy(renderer, blocksLibrary[dragedBlockIndex].textrue, NULL, &textRect);
    }
}

void controlBlockColumnMouseScroll(const int mouseX, const int mouseY, const Sint32 scrollY)
{
    if (isPointInRect(mouseX, mouseY, BLOCKS_COLUMN))
    {
        scrollBlockColumnIndex += scrollY * SCROLL_Y_SENSIVITY;

#ifdef DEBUG_MODE
        std::cout << "ScrollY: " << scrollY << std::endl;
        std::cout << "Scroll Index: " << scrollBlockColumnIndex << " blocksScrollLimit:" << blocksScrollLimit << std::endl;
#endif

        if (scrollBlockColumnIndex < 0)
            scrollBlockColumnIndex = 0;
        if (scrollBlockColumnIndex > blocksScrollLimit)
            scrollBlockColumnIndex = blocksScrollLimit;
    }
}

/*
--------------------------------------------
This function is for controling mouse button down on BLock columne
--------------------------------------------
- drag & drop
*/
void controlBlockColumnClickDown(const int mouseX, const int mouseY)
{
    // check if signal is about block columne
    if (!isPointInRect(mouseX, mouseY, BLOCKS_COLUMN))
        return;

    const int heightFromOffset = mouseY - BLOCKS_COLUMN.y + scrollBlockColumnIndex; // This is distance from first offset of the blocks columne height

    int blocksTotalHeight = 5, selectBlockIndex = -1;
    for (int i = 0; i < BLOCKS_COUNT; i++)
    {
        if (blocksLibrary[i].categoryId == selectedCategoryId)
        {
            if (heightFromOffset >= blocksTotalHeight && heightFromOffset <= (blocksTotalHeight + blocksLibrary[i].height))
            {
                selectBlockIndex = i;
                break;
            }

            blocksTotalHeight += blocksLibrary[i].height + 5;
        }
    }

    if (selectBlockIndex == -1) // None of blocks selected
        return;

    // std::cout << blocksLibrary[selectBlockIndex].label << std::endl;

    // now an item is selected so we mentain it's drag routin
    dragedBlockIndex = selectBlockIndex;
    isBLockDraged = true;
}

/*
--------------------------------------------
This function is for controling mouse button up on BLock columne
--------------------------------------------
- drag & drop
*/
void controlBlockColumnClickUp(const int mouseX, const int mouseY)
{
    // check if signal is about block columne
    if (!isPointInRect(mouseX, mouseY, BLOCKS_COLUMN))
        return;

    // Cancel Drag process
    if (isBLockDraged)
    {
        isBLockDraged = false;
    }
}