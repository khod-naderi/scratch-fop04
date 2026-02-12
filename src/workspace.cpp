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
#include "blocks_library.h"
#include "ui.h"
#include "block.h"
#include "codespace.h"
#include "block_instance.h"
#include <cmath>
#include <iostream>

// Legacy system
std::vector<CodeBlock> activeCodeBlocks;
int lastId = 0;

// New system
CodeSpace workspaceCodeSpace;

// moving items properties
bool isMovingItem = false;
int movingTopSelectedItemIndex;
std::vector<int> movingStringIndesis;

// Scroll workspace properties
bool isScrollingWorkspace = false;
int mouseX0, mouseY0;
int scrollOffsetX = 0;
int scrollOffsetY = 0;

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
    // Check in new CodeSpace system
    for (int i = 0; i < MAX_INSTANCES; i++)
    {
        if (!workspaceCodeSpace.instanceUsed[i])
            continue;

        BlockInstance inst = workspaceCodeSpace.instances[i];
        Block def = blocksLibrary[inst.defenitionId];

        // Skip if this block already have a next connection or can't have bottom connection
        if (inst.nextId != -1 || !def.canHaveBottomConnection)
            continue;

        const int blc_x = inst.posX;
        const int blc_y = inst.posY + def.baseHeight;
        if (std::sqrt(std::pow((blc_x - TL_X), 2) + std::pow((blc_y - TL_Y), 2)) <= CONNECTION_MINIMUM_DISTANCE)
        {
            return inst.instanceId;
        }
    }

    // Fallback to legacy system for compatibility
    for (const CodeBlock &item : activeCodeBlocks)
    {
        if (item.bottomId != -1 || !blocksLibrary[item.blockMaster].canHaveBottomConnection)
            continue;

        const int blc_x = item.posX;
        const int blc_y = item.posY + blocksLibrary[item.blockMaster].baseHeight;
        if (std::sqrt(std::pow((blc_x - TL_X), 2) + std::pow((blc_y - TL_Y), 2)) <= CONNECTION_MINIMUM_DISTANCE)
        {
            return item.id;
        }
    }

    return -1;
}

/*
-----------------------
This function will search for item with specific id in legacy system
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
-----------------------
This function will search for BlockInstance with specific id in new system
-----------------------
return pointer to BlockInstance or NULL if not found
*/
BlockInstance *findBlockInstanceById(int id)
{
    return codespaceGetInstance(workspaceCodeSpace, id);
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
    const int relX = mouseX - WORKSPACE_COLUMN.x - scrollOffsetX;
    const int relY = mouseY - WORKSPACE_COLUMN.y - scrollOffsetY;

    // Add draged item to screen
    if (isBLockDraged)
    {
        isBLockDraged = false;

        // Create new item
        int posX = relX - blocksLibrary[dragedBlockIndex].baseWidth / 2;
        int posY = relY - blocksLibrary[dragedBlockIndex].baseHeight / 2;

        int newInstanceId = codespaceCreateInstance(workspaceCodeSpace, dragedBlockIndex, posX, posY);

        if (newInstanceId != -1 && blocksLibrary[dragedBlockIndex].canHaveTopConnection)
        {
            int topId = isItemNearTopToConnect(posX, posY);
            if (topId != -1) // connect Item to it's top item
            {
                BlockInstance *newInst = findBlockInstanceById(newInstanceId);
                BlockInstance *topInst = findBlockInstanceById(topId);

                if (newInst && topInst)
                {
                    Block topDef = blocksLibrary[topInst->defenitionId];
                    newInst->posX = topInst->posX;
                    newInst->posY = topInst->posY + topDef.baseHeight;

                    codespaceAttachNext(workspaceCodeSpace, topId, newInstanceId);
                }
            }
        }

        // Legacy system - create old CodeBlock for compatibility
        CodeBlock newItem;
        newItem.posX = posX;
        newItem.posY = posY;
        newItem.blockMaster = dragedBlockIndex;
        newItem.id = lastId++;
        newItem.bottomId = -1;
        newItem.topId = -1;
        activeCodeBlocks.push_back(newItem);
    }

    // fix moving string - handle both new and legacy systems
    if (isMovingItem)
    {
        // Handle legacy CodeBlock system
        if (movingTopSelectedItemIndex != -1 && movingTopSelectedItemIndex < activeCodeBlocks.size())
        {
            if (blocksLibrary[activeCodeBlocks[movingTopSelectedItemIndex].blockMaster].canHaveTopConnection)
            {
                activeCodeBlocks[movingTopSelectedItemIndex].topId = isItemNearTopToConnect(activeCodeBlocks[movingTopSelectedItemIndex].posX, activeCodeBlocks[movingTopSelectedItemIndex].posY);
                if (activeCodeBlocks[movingTopSelectedItemIndex].topId != -1)
                {
                    int topItemIndex = foundItemIndexById(activeCodeBlocks[movingTopSelectedItemIndex].topId);
                    if (topItemIndex != -1)
                    {
                        activeCodeBlocks[topItemIndex].bottomId = activeCodeBlocks[movingTopSelectedItemIndex].id;

                        int totalHeight = 0;
                        for (int i : movingStringIndesis)
                        {
                            if (i < activeCodeBlocks.size())
                            {
                                activeCodeBlocks[i].posX = activeCodeBlocks[topItemIndex].posX;
                                activeCodeBlocks[i].posY = activeCodeBlocks[topItemIndex].posY + blocksLibrary[activeCodeBlocks[topItemIndex].blockMaster].baseHeight + totalHeight;
                                totalHeight += blocksLibrary[activeCodeBlocks[i].blockMaster].baseHeight;
                            }
                        }
                    }
                }
            }
        }

        // TODO: Handle BlockInstance system moving and reconnection
        // For now, BlockInstances are detached when moved and need manual reconnection

        isMovingItem = false;
        movingStringIndesis.clear();
        movingTopSelectedItemIndex = -1;
    }

    if (isScrollingWorkspace) // Disable Scrolling
    {
        isScrollingWorkspace = false;
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

    bool isClickedOnItem = false;

    // Check BlockInstances first (new system)
    for (int i = 0; i < MAX_INSTANCES && !isClickedOnItem; i++)
    {
        if (!workspaceCodeSpace.instanceUsed[i])
            continue;

        BlockInstance inst = workspaceCodeSpace.instances[i];
        Block def = blocksLibrary[inst.defenitionId];

        SDL_Rect itemRect = {
            WORKSPACE_COLUMN.x + inst.posX + scrollOffsetX,
            WORKSPACE_COLUMN.y + inst.posY + scrollOffsetY,
            def.baseWidth,
            def.baseHeight,
        };

        if (isPointInRect(mouseX, mouseY, itemRect)) // found grabbed item
        {
            isMovingItem = true;
            isClickedOnItem = true;

            // Detach from parent connections
            codespaceDetach(workspaceCodeSpace, inst.instanceId);

            // TODO: Implement full chain moving for BlockInstance system

            break;
        }
    }

    // Check legacy CodeBlocks if no BlockInstance was clicked
    if (!isClickedOnItem)
    {
        for (CodeBlock &item : activeCodeBlocks)
        {
            SDL_Rect itemRect = {
                WORKSPACE_COLUMN.x + item.posX + scrollOffsetX,
                WORKSPACE_COLUMN.y + item.posY + scrollOffsetY,
                blocksLibrary[item.blockMaster].baseWidth,
                blocksLibrary[item.blockMaster].baseHeight,
            };

            if (isPointInRect(mouseX, mouseY, itemRect)) // found grabbed item
            {
                isMovingItem = true;
                isClickedOnItem = true;
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

    // Scroll workspace screen
    if (!isClickedOnItem && !isScrollingWorkspace) // clicked on emtpy space
    {
        isScrollingWorkspace = true;
        mouseX0 = mouseX;
        mouseY0 = mouseY;
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
        const int relX = mouseX - WORKSPACE_COLUMN.x - scrollOffsetX;
        const int relY = mouseY - WORKSPACE_COLUMN.y - scrollOffsetY;

        int totalHeight = 0;
        for (int i : movingStringIndesis)
        {
            activeCodeBlocks[i].posX = relX - blocksLibrary[dragedBlockIndex].baseWidth / 2;
            activeCodeBlocks[i].posY = relY - blocksLibrary[dragedBlockIndex].baseHeight / 2 + totalHeight;
            totalHeight += blocksLibrary[activeCodeBlocks[i].blockMaster].baseHeight;
        }
    }

    if (isScrollingWorkspace)
    {
        scrollOffsetX += mouseX - mouseX0;
        scrollOffsetY += mouseY - mouseY0;
        mouseX0 = mouseX;
        mouseY0 = mouseY;
    }
}

void drawWorkspaceScreen(SDL_Renderer *renderer, TTF_Font *font, const int mouseX, const int mouseY)
{

    // Draw shadow for connecting draged item
    if (isBLockDraged)
    {
        const int TL_X = mouseX - blocksLibrary[dragedBlockIndex].baseWidth / 2 - WORKSPACE_COLUMN.x - scrollOffsetX;
        const int TL_Y = mouseY - blocksLibrary[dragedBlockIndex].baseHeight / 2 - WORKSPACE_COLUMN.y - scrollOffsetY;

        if (blocksLibrary[dragedBlockIndex].canHaveTopConnection)
        {
            int nearItemIndex = foundItemIndexById(isItemNearTopToConnect(TL_X, TL_Y));
            if (nearItemIndex != -1) // there is a near connectable item
            {
                // Draw draged item centre on mouse pointer
                SDL_Rect dragedItemRect = {
                    WORKSPACE_COLUMN.x + activeCodeBlocks[nearItemIndex].posX + scrollOffsetX,
                    WORKSPACE_COLUMN.y + activeCodeBlocks[nearItemIndex].posY + blocksLibrary[activeCodeBlocks[nearItemIndex].blockMaster].baseHeight + scrollOffsetY,
                    blocksLibrary[dragedBlockIndex].baseWidth,
                    blocksLibrary[dragedBlockIndex].baseHeight,
                };

                // Fill item background
                SDL_SetRenderDrawColor(renderer, color_softGray);
                SDL_RenderFillRect(renderer, &dragedItemRect);
            }
        }
    }

    // Draw shadow for connecting moving string
    if (isMovingItem)
    {
        if (blocksLibrary[activeCodeBlocks[movingTopSelectedItemIndex].blockMaster].canHaveTopConnection)
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
                        WORKSPACE_COLUMN.x + activeCodeBlocks[topItemIndex].posX + scrollOffsetX,
                        WORKSPACE_COLUMN.y + activeCodeBlocks[topItemIndex].posY + blocksLibrary[activeCodeBlocks[topItemIndex].blockMaster].baseHeight + totalHeight + scrollOffsetY,
                        blocksLibrary[activeCodeBlocks[i].blockMaster].baseWidth,
                        blocksLibrary[activeCodeBlocks[i].blockMaster].baseHeight,
                    };

                    totalHeight += blocksLibrary[activeCodeBlocks[i].blockMaster].baseHeight;

                    // Fill item background
                    SDL_SetRenderDrawColor(renderer, color_softGray);
                    SDL_RenderFillRect(renderer, &dragedItemRect);
                }
            }
        }
    }

    // Draw BlockInstances from new system
    for (int i = 0; i < MAX_INSTANCES; i++)
    {
        if (!workspaceCodeSpace.instanceUsed[i])
            continue;

        BlockInstance inst = workspaceCodeSpace.instances[i];
        Block def = blocksLibrary[inst.defenitionId];

        SDL_Rect itemRect = {
            WORKSPACE_COLUMN.x + inst.posX + scrollOffsetX,
            WORKSPACE_COLUMN.y + inst.posY + scrollOffsetY,
            def.baseWidth,
            def.baseHeight,
        };

        // item background
        if (!isBLockDraged && isPointInRect(mouseX, mouseY, itemRect))
        {
            SDL_SetRenderDrawColor(renderer, colorDim(categories[def.categoryId].color));
        }
        else
        {
            SDL_SetRenderDrawColor(renderer, categories[def.categoryId].color);
        }
        SDL_RenderFillRect(renderer, &itemRect);

        // item border
        SDL_SetRenderDrawColor(renderer, color_black);
        SDL_RenderDrawRect(renderer, &itemRect);

        // item text
        int tw, th;
        SDL_Texture *texture = renderText(renderer, font, def.label, color_white);
        SDL_QueryTexture(texture, NULL, NULL, &tw, &th);
        SDL_Rect textRect = {
            itemRect.x + (itemRect.w - tw) / 2,
            itemRect.y + (itemRect.h - th) / 2,
            tw,
            th,
        };
        SDL_RenderCopy(renderer, texture, NULL, &textRect);
    }

    // Draw legacy CodeBlocks for compatibility
    for (const CodeBlock &item : activeCodeBlocks)
    {
        SDL_Rect itemRect = {
            WORKSPACE_COLUMN.x + item.posX + scrollOffsetX,
            WORKSPACE_COLUMN.y + item.posY + scrollOffsetY,
            blocksLibrary[item.blockMaster].baseWidth,
            blocksLibrary[item.blockMaster].baseHeight,
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
        SDL_Texture *texture = renderText(renderer, font, blocksLibrary[item.blockMaster].label, color_white);
        SDL_QueryTexture(texture, NULL, NULL, &tw, &th);
        SDL_Rect textRect = {
            itemRect.x + (itemRect.w - tw) / 2,
            itemRect.y + (itemRect.h - th) / 2,
            tw,
            th,
        };
        SDL_RenderCopy(renderer, texture, NULL, &textRect);
    }

    return;
}

int workspaceScreenInit(SDL_Renderer *renderer, TTF_Font *font)
{
    // Initialize the new CodeSpace system
    codespaceInit(workspaceCodeSpace);
    return 0;
}

int createBlockInstanceInWorkspace(int definitionId, int posX, int posY)
{
    return codespaceCreateInstance(workspaceCodeSpace, definitionId, posX, posY);
}

void attachBlocksSequentially(int topId, int bottomId)
{
    codespaceAttachNext(workspaceCodeSpace, topId, bottomId);
}

void detachBlockFromWorkspace(int id)
{
    codespaceDetach(workspaceCodeSpace, id);
}

void removeBlockFromWorkspace(int id)
{
    codespaceRemoveInstance(workspaceCodeSpace, id);
}