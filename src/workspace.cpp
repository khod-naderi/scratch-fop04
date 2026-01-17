/*
-------------------------------------------
This CPP file is for managing code view named workspace
-------------------------------------------
*/
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "workspace.h"
#include "color.h"
#include "ui.h"
#include "block.h"
#include <cmath>
#include <iostream>

std::vector<CodeBlock> activeCodeBlocks;
int lastId = 0;

// moving items properties
bool isMovingItem = false;
int movingTopSelectedItemIndex;
std::vector<int> movingStringIndesis;

/*
-------------------------------
This function check if any other item that does not have bottom connection is near the top this item.
-------------------------------
@input:
TL: top left
TL_X: x position of top left corner of this item (related to workspace cordinate).
TL_Y: y position of top left corner of this item (related to workspace cordinate).
@output:
if yes, it's will return Id of that item,
if not, it's will return -1
*/
int isItemNearTopToConnect(const int TL_X, const int TL_Y)
{
    for (const CodeBlock &item : activeCodeBlocks)
    {
        if (item.bottomId != -1)
            continue;

        const int blc_x = item.posX;
        const int blc_y = item.posY + blocksLibrary[item.blockMaster].height;
        if (std::sqrt(std::pow((blc_x - TL_X), 2) + std::pow((blc_y - TL_Y), 2)) <= CONNECTION_MINIMUM_DISTANCE)
        {
            return item.id;
        }
    }

    return -1;
}

/*
-----------------------
This function will search for item with specific id
-----------------------
return -1 if not found
*/
int foundItemIndexById(int id)
{
    for (int i = 0; i < activeCodeBlocks.size(); i++)
    {
        if (activeCodeBlocks[i].id == id)
            return i;
    }

    return -1;
}

/*
--------------------------------------------
This function is for controling mouse button up on WorkSpace
--------------------------------------------
- drag & drop
*/
void controlWorkspaceClickUp(const int mouseX, const int mouseY)
{
    // check if signal is about workspace
    if (!isPointInRect(mouseX, mouseY, WORKSPACE_COLUMN))
        return;

    // relative position to workspace cordinate
    const int relX = mouseX - WORKSPACE_COLUMN.x;
    const int relY = mouseY - WORKSPACE_COLUMN.y;

    // Add draged item to screen
    if (isBLockDraged)
    {
        isBLockDraged = false;

        // Create new item
        CodeBlock newItem;
        newItem.posX = relX - blocksLibrary[dragedBlockIndex].width / 2;
        newItem.posY = relY - blocksLibrary[dragedBlockIndex].height / 2;
        newItem.blockMaster = dragedBlockIndex;
        newItem.id = lastId++;
        newItem.bottomId = -1;
        newItem.topId = isItemNearTopToConnect(newItem.posX, newItem.posY);
        if (newItem.topId != -1) // connect Item to it's top item
        {
            int topItemIndex = foundItemIndexById(newItem.topId);
            newItem.posX = activeCodeBlocks[topItemIndex].posX;
            newItem.posY = activeCodeBlocks[topItemIndex].posY + blocksLibrary[activeCodeBlocks[topItemIndex].blockMaster].height;

            activeCodeBlocks[topItemIndex].bottomId = newItem.id;
        }

        activeCodeBlocks.push_back(newItem);
    }

    // fix moving string
    if (isMovingItem)
    {
        activeCodeBlocks[movingTopSelectedItemIndex].topId = isItemNearTopToConnect(activeCodeBlocks[movingTopSelectedItemIndex].posX, activeCodeBlocks[movingTopSelectedItemIndex].posY);
        if (activeCodeBlocks[movingTopSelectedItemIndex].topId != -1)
        {
            int topItemIndex = foundItemIndexById(activeCodeBlocks[movingTopSelectedItemIndex].topId);
            activeCodeBlocks[topItemIndex].bottomId = activeCodeBlocks[movingTopSelectedItemIndex].id;

            int totalHeight = 0;
            for (int i : movingStringIndesis)
            {
                activeCodeBlocks[i].posX = activeCodeBlocks[topItemIndex].posX;
                activeCodeBlocks[i].posY = activeCodeBlocks[topItemIndex].posY + blocksLibrary[activeCodeBlocks[topItemIndex].blockMaster].height + totalHeight;
                totalHeight += blocksLibrary[activeCodeBlocks[i].blockMaster].height;
            }
        }

        isMovingItem = false;
        movingStringIndesis.clear();
        movingTopSelectedItemIndex = -1;
    }

    return;
}

/*
--------------------------------------------
This function is for controling mouse button down on WorkSpace
--------------------------------------------
- drag & drop
- move items
*/
void controlWorkspaceClickDown(const int mouseX, const int mouseY)
{
    // check if signal is about workspace
    if (!isPointInRect(mouseX, mouseY, WORKSPACE_COLUMN))
        return;

    for (CodeBlock &item : activeCodeBlocks)
    {
        SDL_Rect itemRect = {
            WORKSPACE_COLUMN.x + item.posX,
            WORKSPACE_COLUMN.y + item.posY,
            blocksLibrary[item.blockMaster].width,
            blocksLibrary[item.blockMaster].height,
        };

        if (isPointInRect(mouseX, mouseY, itemRect)) // found grabed item
        {
            isMovingItem = true;
            movingTopSelectedItemIndex = foundItemIndexById(item.id);

            // disconnect from it's top
            int itemTopIndex = foundItemIndexById(item.topId);
            if (itemTopIndex != -1)
                activeCodeBlocks[itemTopIndex].bottomId = -1;
            item.topId = -1;

            // found moving string
            movingStringIndesis.clear();
            movingStringIndesis.push_back(movingTopSelectedItemIndex);
            for (int i = movingTopSelectedItemIndex;;)
            {
                i = foundItemIndexById(activeCodeBlocks[i].bottomId);
                if (i == -1)
                    break;
                movingStringIndesis.push_back(i);
            }

            break;
        }
    }
}

/*
--------------------------------------------
This function is for controling mouse motion on WorkSpace
--------------------------------------------

- move items
*/
void controlWorkspaceMouseMotion(const int mouseX, const int mouseY)
{
    // check if signal is about workspace
    if (!isPointInRect(mouseX, mouseY, WORKSPACE_COLUMN))
        return;

    // update moving string
    if (isMovingItem)
    {
        // relative position to workspace cordinate
        const int relX = mouseX - WORKSPACE_COLUMN.x;
        const int relY = mouseY - WORKSPACE_COLUMN.y;

        int totalHeight = 0;
        for (int i : movingStringIndesis)
        {
            activeCodeBlocks[i].posX = relX - blocksLibrary[dragedBlockIndex].width / 2;
            activeCodeBlocks[i].posY = relY - blocksLibrary[dragedBlockIndex].height / 2 + totalHeight;
            totalHeight += blocksLibrary[activeCodeBlocks[i].blockMaster].height;
        }
    }
}

void drawWorkspaceScreen(SDL_Renderer *renderer, TTF_Font *font, const int mouseX, const int mouseY)
{
    for (const CodeBlock &item : activeCodeBlocks)
    {
        SDL_Rect itemRect = {
            WORKSPACE_COLUMN.x + item.posX,
            WORKSPACE_COLUMN.y + item.posY,
            blocksLibrary[item.blockMaster].width,
            blocksLibrary[item.blockMaster].height,
        };

        // item background
        if (!isBLockDraged && isPointInRect(mouseX, mouseY, itemRect))
        {
            SDL_SetRenderDrawColor(renderer, colorDim(categories[blocksLibrary[item.blockMaster].categoryId].color));
        }
        else
        {
            SDL_SetRenderDrawColor(renderer, categories[blocksLibrary[item.blockMaster].categoryId].color);
        }
        SDL_RenderFillRect(renderer, &itemRect);

        // item border
        SDL_SetRenderDrawColor(renderer, color_black);
        SDL_RenderDrawRect(renderer, &itemRect);

        // item text
        int tw, th;
        SDL_QueryTexture(blocksLibrary[item.blockMaster].textrue, NULL, NULL, &tw, &th);
        SDL_Rect textRect = {
            itemRect.x + (itemRect.w - tw) / 2,
            itemRect.y + (itemRect.h - th) / 2,
            tw,
            th,
        };
        SDL_RenderCopy(renderer, blocksLibrary[item.blockMaster].textrue, NULL, &textRect);
    }

    // Draw shadow for connecting draged item
    if (isBLockDraged)
    {
        const int TL_X = mouseX - blocksLibrary[dragedBlockIndex].width / 2 - WORKSPACE_COLUMN.x;
        const int TL_Y = mouseY - blocksLibrary[dragedBlockIndex].height / 2 - WORKSPACE_COLUMN.y;

        int nearItemIndex = foundItemIndexById(isItemNearTopToConnect(TL_X, TL_Y));
        if (nearItemIndex != -1) // there is a near connectable item
        {
            // Draw draged item centre on mouse pointer
            SDL_Rect dragedItemRect = {
                WORKSPACE_COLUMN.x + activeCodeBlocks[nearItemIndex].posX,
                WORKSPACE_COLUMN.y + activeCodeBlocks[nearItemIndex].posY + blocksLibrary[activeCodeBlocks[nearItemIndex].blockMaster].height,
                blocksLibrary[dragedBlockIndex].width,
                blocksLibrary[dragedBlockIndex].height,
            };

            // Fill item background
            SDL_SetRenderDrawColor(renderer, color_softGray);
            SDL_RenderFillRect(renderer, &dragedItemRect);
        }
    }

    // Draw shadow for connecting moving string
    if (isMovingItem)
    {
        int topItemId = isItemNearTopToConnect(activeCodeBlocks[movingTopSelectedItemIndex].posX, activeCodeBlocks[movingTopSelectedItemIndex].posY);
        if (topItemId != -1) //  there is a near connectable item
        {
            int topItemIndex = foundItemIndexById(topItemId);

            int totalHeight = 0;
            for (int i : movingStringIndesis)
            {
                // Draw draged item centre on mouse pointer
                SDL_Rect dragedItemRect = {
                    WORKSPACE_COLUMN.x + activeCodeBlocks[topItemIndex].posX,
                    WORKSPACE_COLUMN.y + activeCodeBlocks[topItemIndex].posY + blocksLibrary[activeCodeBlocks[topItemIndex].blockMaster].height + totalHeight,
                    blocksLibrary[i].width,
                    blocksLibrary[i].height,
                };

                totalHeight += blocksLibrary[activeCodeBlocks[i].blockMaster].height;

                // Fill item background
                SDL_SetRenderDrawColor(renderer, color_softGray);
                SDL_RenderFillRect(renderer, &dragedItemRect);
            }
        }
    }

    return;
}

int workspaceScreenInit(SDL_Renderer *renderer, TTF_Font *font)
{
    return 0;
}