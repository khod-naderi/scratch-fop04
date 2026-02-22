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
#include "blocks_library.h"
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

int blockColumnInit(SDL_Renderer *renderer, TTF_Font *font)
{
    return 0;
}

int blocksScrollLimit = 0;

std::string lprintf(const char *format, const SlotDefinition slots[])
{
    std::string out = "";
    int j = 0;
    for (size_t i = 0; i < strlen(format); i++)
    {
        if (format[i] == '%')
        {
            i++;
            if (format[i] == 's')
            {
                out += slots[j].defaultValue.asString();
                j++;
                continue;
            }
        }
        out.push_back(format[i]);
    }
    return out;
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
    for (int i = 0; i < BLOCKS_COUNT; i++)
    {
        if (blocksLibrary[i].categoryId == selectedCategoryId)
        {
            SDL_Rect itemRect = {
                BLOCKS_COLUMN.x + 5,
                BLOCKS_COLUMN.y + blocksTotalHeight - scrollBlockColumnIndex,
                blocksLibrary[i].baseWidth,
                blocksLibrary[i].baseHeight,
            };
            blocksTotalHeight += blocksLibrary[i].baseHeight + 5;
            block_count++;

            // skip of not in the scrolled section.
            if ((blocksTotalHeight - scrollBlockColumnIndex < 0) || (blocksTotalHeight - scrollBlockColumnIndex > MAIN_WINDOW_HEIGHT))
            {
                continue;
            }
            visibleBlockCount++;

            // Background
            if (!isBLockDraged && isPointInRect(mouseX, mouseY, itemRect))
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
            std::string labelBuffer = lprintf(blocksLibrary[i].label, blocksLibrary[i].slots);
            SDL_Texture *texture = renderText(renderer, font, labelBuffer.c_str(), color_white);
            SDL_QueryTexture(texture, NULL, NULL, &tw, &th);
            SDL_Rect textRect = {
                itemRect.x + (itemRect.w - tw) / 2,
                itemRect.y + (itemRect.h - th) / 2,
                tw,
                th,
            };
            SDL_RenderCopy(renderer, texture, NULL, &textRect);
        }
    }
    blocksScrollLimit = blocksTotalHeight - scrollBlockColumnIndex - (blocksLibrary[BLOCKS_COUNT - 1].baseHeight);

    // Draw draged block
    if (isBLockDraged)
    {
        // Draw draged item centre on mouse pointer
        SDL_Rect dragedItemRect = {
            mouseX - blocksLibrary[dragedBlockIndex].baseWidth / 2,
            mouseY - blocksLibrary[dragedBlockIndex].baseHeight / 2,
            blocksLibrary[dragedBlockIndex].baseWidth,
            blocksLibrary[dragedBlockIndex].baseHeight,
        };

        // Fill item background
        SDL_SetRenderDrawColor(renderer, categories[blocksLibrary[dragedBlockIndex].categoryId].color);
        SDL_RenderFillRect(renderer, &dragedItemRect);

        // item border
        SDL_SetRenderDrawColor(renderer, color_black);
        SDL_RenderDrawRect(renderer, &dragedItemRect);

        // item text
        int tw, th;
        std::string labelBuffer = lprintf(blocksLibrary[dragedBlockIndex].label, blocksLibrary[dragedBlockIndex].slots);
        SDL_Texture *texture = renderText(renderer, font, labelBuffer.c_str(), color_white);
        SDL_QueryTexture(texture, NULL, NULL, &tw, &th);
        SDL_Rect textRect = {
            dragedItemRect.x + (dragedItemRect.w - tw) / 2,
            dragedItemRect.y + (dragedItemRect.h - th) / 2,
            tw,
            th,
        };
        SDL_RenderCopy(renderer, texture, NULL, &textRect);
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
            if (heightFromOffset >= blocksTotalHeight && heightFromOffset <= (blocksTotalHeight + blocksLibrary[i].baseHeight))
            {
                selectBlockIndex = i;
                break;
            }

            blocksTotalHeight += blocksLibrary[i].baseHeight + 5;
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
    // check if signal is not about workspace
    if (!isPointInRect(mouseX, mouseY, WORKSPACE_COLUMN))
    {
        // Cancel Drag process
        if (isBLockDraged)
        {
            isBLockDraged = false;
        }
    }
}