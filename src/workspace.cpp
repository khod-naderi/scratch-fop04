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

CodeSpace workspaceCodeSpace;

// moving items properties
bool isMovingItem = false;
int movingInstanceId = -1;
std::vector<int> movingChainIds;

// Scroll workspace properties
bool isScrollingWorkspace = false;
int mouseX0, mouseY0;
int scrollOffsetX = 0;
int scrollOffsetY = 0;

/*
-------------------------------
This function check if any BlockInstance that can have bottom connection is near the top of this item.
-------------------------------
@input:
TL_X: x position of top left corner of this item (related to workspace cordinate).
TL_Y: y position of top left corner of this item (related to workspace cordinate).
@output:
if yes, it will return instanceId of that item,
if not, it will return -1
*/
int isItemNearTopToConnect(const int TL_X, const int TL_Y)
{
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
-----------------------
Build a chain of connected blocks starting from the given block
-----------------------
*/
void buildMovingChain(int startId, std::vector<int> &chain)
{
    chain.clear();
    int currentId = startId;

    while (currentId != -1)
    {
        chain.push_back(currentId);
        BlockInstance *inst = findBlockInstanceById(currentId);
        if (!inst)
            break;
        currentId = inst->nextId;
    }
}

/*
-----------------------
Update positions of all blocks in a chain
-----------------------
*/
void updateChainPositions(const std::vector<int> &chain, int startX, int startY)
{
    int currentY = startY;

    for (int id : chain)
    {
        BlockInstance *inst = findBlockInstanceById(id);
        if (!inst)
            continue;

        Block def = blocksLibrary[inst->defenitionId];
        inst->posX = startX;
        inst->posY = currentY;
        currentY += def.baseHeight;
    }
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
    }

    // Fix moving chain
    if (isMovingItem && movingInstanceId != -1)
    {
        BlockInstance *movingInst = findBlockInstanceById(movingInstanceId);
        if (movingInst && blocksLibrary[movingInst->defenitionId].canHaveTopConnection)
        {
            int topId = isItemNearTopToConnect(movingInst->posX, movingInst->posY);
            if (topId != -1)
            {
                BlockInstance *topInst = findBlockInstanceById(topId);
                if (topInst)
                {
                    Block topDef = blocksLibrary[topInst->defenitionId];
                    int newX = topInst->posX;
                    int newY = topInst->posY + topDef.baseHeight;

                    updateChainPositions(movingChainIds, newX, newY);
                    codespaceAttachNext(workspaceCodeSpace, topId, movingInstanceId);
                }
            }
        }

        isMovingItem = false;
        movingChainIds.clear();
        movingInstanceId = -1;
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

    // Check BlockInstances
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
            movingInstanceId = inst.instanceId;

            // Detach from parent connections
            codespaceDetach(workspaceCodeSpace, inst.instanceId);

            // Build the chain of connected blocks
            buildMovingChain(movingInstanceId, movingChainIds);

            break;
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

    // update moving chain
    if (isMovingItem && movingInstanceId != -1)
    {
        // relative position to workspace coordinate
        const int relX = mouseX - WORKSPACE_COLUMN.x - scrollOffsetX;
        const int relY = mouseY - WORKSPACE_COLUMN.y - scrollOffsetY;

        BlockInstance *movingInst = findBlockInstanceById(movingInstanceId);
        if (movingInst)
        {
            Block def = blocksLibrary[movingInst->defenitionId];
            int newX = relX - def.baseWidth / 2;
            int newY = relY - def.baseHeight / 2;

            updateChainPositions(movingChainIds, newX, newY);
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
            int nearItemId = isItemNearTopToConnect(TL_X, TL_Y);
            if (nearItemId != -1) // there is a near connectable item
            {
                BlockInstance *nearInst = findBlockInstanceById(nearItemId);
                if (nearInst)
                {
                    Block nearDef = blocksLibrary[nearInst->defenitionId];

                    // Draw dragged item shadow
                    SDL_Rect dragedItemRect = {
                        WORKSPACE_COLUMN.x + nearInst->posX + scrollOffsetX,
                        WORKSPACE_COLUMN.y + nearInst->posY + nearDef.baseHeight + scrollOffsetY,
                        blocksLibrary[dragedBlockIndex].baseWidth,
                        blocksLibrary[dragedBlockIndex].baseHeight,
                    };

                    // Fill item background
                    SDL_SetRenderDrawColor(renderer, color_softGray);
                    SDL_RenderFillRect(renderer, &dragedItemRect);
                }
            }
        }
    }

    // Draw shadow for connecting moving chain
    if (isMovingItem && movingInstanceId != -1)
    {
        BlockInstance *movingInst = findBlockInstanceById(movingInstanceId);
        if (movingInst && blocksLibrary[movingInst->defenitionId].canHaveTopConnection)
        {
            int topItemId = isItemNearTopToConnect(movingInst->posX, movingInst->posY);
            if (topItemId != -1) //  there is a near connectable item
            {
                BlockInstance *topInst = findBlockInstanceById(topItemId);
                if (topInst)
                {
                    Block topDef = blocksLibrary[topInst->defenitionId];
                    int totalHeight = 0;

                    for (int id : movingChainIds)
                    {
                        BlockInstance *chainInst = findBlockInstanceById(id);
                        if (!chainInst)
                            continue;

                        Block chainDef = blocksLibrary[chainInst->defenitionId];

                        // Draw shadow for each block in chain
                        SDL_Rect shadowRect = {
                            WORKSPACE_COLUMN.x + topInst->posX + scrollOffsetX,
                            WORKSPACE_COLUMN.y + topInst->posY + topDef.baseHeight + totalHeight + scrollOffsetY,
                            chainDef.baseWidth,
                            chainDef.baseHeight,
                        };

                        totalHeight += chainDef.baseHeight;

                        // Fill item background
                        SDL_SetRenderDrawColor(renderer, color_softGray);
                        SDL_RenderFillRect(renderer, &shadowRect);
                    }
                }
            }
        }
    }

    // Draw BlockInstances
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

    return;
}

int workspaceScreenInit(SDL_Renderer *renderer, TTF_Font *font)
{
    // Initialize the CodeSpace system
    codespaceInit(workspaceCodeSpace);
    return 0;
}

// Helper functions for CodeSpace integration

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